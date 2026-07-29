#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_MONTE_CARLO_TREE_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_MONTE_CARLO_TREE_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

enum class MctsProvenOutcome : int{
    loss = -1,
    draw = 0,
    win = 1,
    unknown = 2
};

struct MctsProgressiveWidening{
    double coefficient = 1.0;
    double exponent = 0.5;
};

template<class Move>
struct AdvancedMonteCarloTreeSearchResult{
    std::optional<Move> move;
    std::size_t visits = 0;
    double average_reward = 0.0;
    std::size_t tree_nodes = 0;
    std::size_t amaf_updates = 0;
    MctsProvenOutcome proven_outcome = MctsProvenOutcome::unknown;
};

template<class State, class Move>
struct MonteCarloRolloutResult{
    State final_state;
    std::vector<Move> moves;
    double reward = 0.0;
    bool terminal = false;
};

template<class Move>
struct NestedMonteCarloSearchResult{
    std::vector<Move> moves;
    double score = -std::numeric_limits<double>::infinity();
    std::size_t rollouts = 0;
};

template<class Move>
struct NrpaSearchResult{
    std::vector<Move> moves;
    std::vector<double> policy;
    double score = -std::numeric_limits<double>::infinity();
    std::size_t rollouts = 0;
};

namespace advanced_monte_carlo_tree_search_internal{

template<class Range>
using range_value_t = std::decay_t<decltype(
    *std::begin(std::declval<std::remove_reference_t<Range>&>())
)>;

template<class Move, class Range>
std::vector<Move> collect_moves(Range&& range){
    std::vector<Move> result;
    if constexpr(std::is_rvalue_reference_v<Range&&>){
        for(auto& move : range) result.emplace_back(std::move(move));
    }else{
        for(const auto& move : range) result.emplace_back(move);
    }
    return result;
}

inline void require_non_negative_finite(double value, const char* message){
    if(value < 0.0 || !std::isfinite(value))[[unlikely]]{
        throw std::invalid_argument(message);
    }
}

inline void require_widening(const MctsProgressiveWidening& widening){
    if(!(widening.coefficient > 0.0)
       || !std::isfinite(widening.coefficient)
       || widening.exponent < 0.0 || widening.exponent > 1.0
       || !std::isfinite(widening.exponent))[[unlikely]]{
        throw std::invalid_argument(
            "progressive widening requires coefficient > 0 and exponent in [0, 1]"
        );
    }
}

inline std::size_t widening_limit(
    const MctsProgressiveWidening& widening,
    std::size_t visits,
    std::size_t maximum
){
    if(maximum == 0) return 0;
    const long double raw = static_cast<long double>(widening.coefficient)
        * std::pow(static_cast<long double>(std::max<std::size_t>(visits, 1)),
                   static_cast<long double>(widening.exponent));
    if(raw >= static_cast<long double>(maximum)) return maximum;
    return std::max<std::size_t>(1, static_cast<std::size_t>(raw));
}

inline MctsProvenOutcome outcome_from_reward(double reward){
    if(reward > 0.0) return MctsProvenOutcome::win;
    if(reward < 0.0) return MctsProvenOutcome::loss;
    return MctsProvenOutcome::draw;
}

enum class deterministic_mode{
    puct,
    rave,
    amaf,
    solver,
    progressive_widening,
    progressive_bias
};

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class Prior, class Bias,
         class RandomEngine>
auto deterministic_search(
    const State& root,
    std::size_t iterations,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    PlayerToMove player_to_move,
    Reward reward,
    Prior prior,
    Bias bias,
    RandomEngine& random_engine,
    deterministic_mode mode,
    double exploration,
    double rave_equivalence,
    MctsProgressiveWidening widening,
    double progressive_bias_weight,
    std::size_t maximum_rollout_depth
){
    using MoveRange = std::invoke_result_t<LegalMoves&, const State&>;
    using Move = range_value_t<MoveRange>;
    using Player = std::decay_t<std::invoke_result_t<PlayerToMove&, const State&>>;
    using Result = AdvancedMonteCarloTreeSearchResult<Move>;

    struct AmafEntry{
        std::size_t visits = 0;
        double reward_sum = 0.0;
    };
    struct Node{
        State state;
        std::optional<Move> incoming_move;
        std::size_t incoming_move_index = 0;
        std::size_t parent = 0;
        std::vector<Move> moves;
        std::vector<std::size_t> unexpanded;
        std::vector<std::size_t> children;
        std::vector<AmafEntry> amaf;
        std::vector<double> priors;
        std::vector<double> biases;
        std::size_t visits = 0;
        double reward_sum = 0.0;
        MctsProvenOutcome proven = MctsProvenOutcome::unknown;
    };

    require_non_negative_finite(
        exploration, "advanced MCTS requires non-negative finite exploration"
    );
    require_non_negative_finite(
        rave_equivalence, "RAVE equivalence must be non-negative and finite"
    );
    require_non_negative_finite(
        progressive_bias_weight,
        "progressive bias weight must be non-negative and finite"
    );
    if(mode == deterministic_mode::progressive_widening){
        require_widening(widening);
    }

    const Player root_player = std::invoke(player_to_move, root);
    auto make_node = [&](State state, std::optional<Move> incoming,
                         std::size_t incoming_index, std::size_t parent){
        Node node{
            std::move(state), std::move(incoming), incoming_index, parent,
            {}, {}, {}, {}, {}, {}, 0, 0.0, MctsProvenOutcome::unknown
        };
        node.moves = collect_moves<Move>(std::invoke(legal_moves, node.state));
        node.unexpanded.resize(node.moves.size());
        node.amaf.resize(node.moves.size());
        node.priors.resize(node.moves.size(), 1.0);
        node.biases.resize(node.moves.size(), 0.0);
        for(std::size_t index = 0; index < node.moves.size(); ++index){
            node.unexpanded[index] = index;
            if(mode == deterministic_mode::puct){
                const double value = static_cast<double>(
                    std::invoke(prior, node.state, node.moves[index])
                );
                if(value < 0.0 || !std::isfinite(value))[[unlikely]]{
                    throw std::invalid_argument(
                        "PUCT priors must be non-negative and finite"
                    );
                }
                node.priors[index] = value;
            }
            if(mode == deterministic_mode::progressive_bias){
                const double value = static_cast<double>(
                    std::invoke(bias, node.state, node.moves[index])
                );
                if(!std::isfinite(value))[[unlikely]]{
                    throw std::invalid_argument(
                        "progressive bias heuristic must be finite"
                    );
                }
                node.biases[index] = value;
            }
        }
        if(mode == deterministic_mode::puct && !node.priors.empty()){
            double sum = 0.0;
            for(const double value : node.priors) sum += value;
            if(!std::isfinite(sum))[[unlikely]]{
                throw std::overflow_error(
                    "the sum of PUCT priors must be finite"
                );
            }
            if(sum == 0.0){
                const double uniform = 1.0 / static_cast<double>(node.priors.size());
                std::fill(node.priors.begin(), node.priors.end(), uniform);
            }else{
                for(double& value : node.priors) value /= sum;
            }
        }
        return node;
    };

    std::vector<Node> nodes;
    nodes.push_back(make_node(root, std::nullopt, 0, 0));
    std::size_t amaf_updates = 0;

    const auto update_proven = [&](Node& node){
        if(mode != deterministic_mode::solver) return;
        if(static_cast<bool>(std::invoke(terminal, node.state))){
            const double value = static_cast<double>(
                std::invoke(reward, node.state, root_player)
            );
            if(!std::isfinite(value))[[unlikely]]{
                throw std::invalid_argument("MCTS-Solver reward must be finite");
            }
            node.proven = outcome_from_reward(value);
            return;
        }
        const bool maximize =
            std::invoke(player_to_move, node.state) == root_player;
        bool all_proven = node.unexpanded.empty();
        bool has_draw = false;
        for(const std::size_t child : node.children){
            const MctsProvenOutcome outcome = nodes[child].proven;
            if((maximize && outcome == MctsProvenOutcome::win)
               || (!maximize && outcome == MctsProvenOutcome::loss)){
                node.proven = outcome;
                return;
            }
            if(outcome == MctsProvenOutcome::unknown) all_proven = false;
            if(outcome == MctsProvenOutcome::draw) has_draw = true;
        }
        if(!all_proven) return;
        if(has_draw) node.proven = MctsProvenOutcome::draw;
        else node.proven = maximize
            ? MctsProvenOutcome::loss : MctsProvenOutcome::win;
    };

    update_proven(nodes[0]);
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        if(mode == deterministic_mode::solver
           && nodes[0].proven != MctsProvenOutcome::unknown) break;

        std::size_t current = 0;
        std::vector<std::size_t> path{0};
        std::vector<Move> played_moves;
        std::vector<Player> played_players;

        while(!static_cast<bool>(std::invoke(terminal, nodes[current].state))){
            const bool may_expand = !nodes[current].unexpanded.empty()
                && (mode != deterministic_mode::progressive_widening
                    || nodes[current].children.size() < widening_limit(
                        widening, nodes[current].visits,
                        nodes[current].moves.size()
                    ));
            if(may_expand){
                std::size_t position = 0;
                if(mode == deterministic_mode::puct){
                    for(std::size_t candidate = 1;
                        candidate < nodes[current].unexpanded.size(); ++candidate){
                        if(nodes[current].priors[
                               nodes[current].unexpanded[position]]
                           < nodes[current].priors[
                               nodes[current].unexpanded[candidate]]){
                            position = candidate;
                        }
                    }
                }else{
                    std::uniform_int_distribution<std::size_t> distribution(
                        0, nodes[current].unexpanded.size() - 1
                    );
                    position = distribution(random_engine);
                }
                const std::size_t move_index =
                    nodes[current].unexpanded[position];
                if(position + 1 != nodes[current].unexpanded.size()){
                    nodes[current].unexpanded[position] =
                        nodes[current].unexpanded.back();
                }
                nodes[current].unexpanded.pop_back();
                const Move move = nodes[current].moves[move_index];
                played_players.push_back(
                    std::invoke(player_to_move, nodes[current].state)
                );
                played_moves.push_back(move);
                State next_state = std::invoke(
                    transition, nodes[current].state, move
                );
                const std::size_t child = nodes.size();
                nodes.push_back(make_node(
                    std::move(next_state), move, move_index, current
                ));
                nodes[current].children.push_back(child);
                current = child;
                path.push_back(current);
                update_proven(nodes[current]);
                break;
            }
            if(nodes[current].children.empty()) break;

            const bool maximize =
                std::invoke(player_to_move, nodes[current].state) == root_player;
            std::size_t selected = nodes[current].children.front();
            double selected_score = -std::numeric_limits<double>::infinity();
            for(const std::size_t child : nodes[current].children){
                const Node& child_node = nodes[child];
                if(mode == deterministic_mode::solver){
                    const auto proof = child_node.proven;
                    if((maximize && proof == MctsProvenOutcome::win)
                       || (!maximize && proof == MctsProvenOutcome::loss)){
                        selected = child;
                        selected_score = std::numeric_limits<double>::infinity();
                        break;
                    }
                }
                const double mean = child_node.visits == 0 ? 0.0
                    : child_node.reward_sum
                        / static_cast<double>(child_node.visits);
                double estimate = mean;
                const std::size_t move_index = child_node.incoming_move_index;
                if(mode == deterministic_mode::rave
                   || mode == deterministic_mode::amaf){
                    const AmafEntry& entry = nodes[current].amaf[move_index];
                    if(entry.visits != 0){
                        const double amaf_mean = entry.reward_sum
                            / static_cast<double>(entry.visits);
                        if(mode == deterministic_mode::amaf){
                            estimate = amaf_mean;
                        }else{
                            const double direct =
                                static_cast<double>(child_node.visits);
                            const double amaf_count =
                                static_cast<double>(entry.visits);
                            const double beta_value = amaf_count
                                / (direct + amaf_count
                                   + 4.0 * direct * amaf_count
                                       * rave_equivalence * rave_equivalence);
                            estimate = (1.0 - beta_value) * mean
                                     + beta_value * amaf_mean;
                        }
                    }
                }
                double score = maximize ? estimate : -estimate;
                if(mode == deterministic_mode::puct){
                    score += exploration
                        * nodes[current].priors[move_index]
                        * std::sqrt(static_cast<double>(
                            std::max<std::size_t>(nodes[current].visits, 1)
                        ))
                        / static_cast<double>(1 + child_node.visits);
                }else{
                    score += exploration * std::sqrt(
                        std::log(static_cast<double>(
                            std::max<std::size_t>(nodes[current].visits, 1)
                        ) + 1.0)
                        / static_cast<double>(
                            std::max<std::size_t>(child_node.visits, 1)
                        )
                    );
                }
                if(mode == deterministic_mode::progressive_bias){
                    const double signed_bias = maximize
                        ? nodes[current].biases[move_index]
                        : -nodes[current].biases[move_index];
                    score += progressive_bias_weight * signed_bias
                           / static_cast<double>(1 + child_node.visits);
                }
                if(mode == deterministic_mode::solver
                   && child_node.proven != MctsProvenOutcome::unknown){
                    score -= 1.0e100;
                }
                if(selected_score < score){
                    selected_score = score;
                    selected = child;
                }
            }
            played_players.push_back(
                std::invoke(player_to_move, nodes[current].state)
            );
            played_moves.push_back(*nodes[selected].incoming_move);
            current = selected;
            path.push_back(current);
        }

        State rollout_state = nodes[current].state;
        for(std::size_t depth = 0; depth < maximum_rollout_depth; ++depth){
            if(static_cast<bool>(std::invoke(terminal, rollout_state))) break;
            auto moves = collect_moves<Move>(
                std::invoke(legal_moves, rollout_state)
            );
            if(moves.empty()) break;
            std::uniform_int_distribution<std::size_t> distribution(
                0, moves.size() - 1
            );
            const Move move = moves[distribution(random_engine)];
            played_players.push_back(
                std::invoke(player_to_move, rollout_state)
            );
            played_moves.push_back(move);
            rollout_state = std::invoke(transition, rollout_state, move);
        }
        const double outcome = static_cast<double>(
            std::invoke(reward, rollout_state, root_player)
        );
        if(!std::isfinite(outcome))[[unlikely]]{
            throw std::invalid_argument("advanced MCTS reward must be finite");
        }

        for(const std::size_t node_index : path){
            ++nodes[node_index].visits;
            nodes[node_index].reward_sum += outcome;
        }
        if(mode == deterministic_mode::rave
           || mode == deterministic_mode::amaf){
            for(std::size_t path_index = 0; path_index < path.size();
                ++path_index){
                Node& node = nodes[path[path_index]];
                const Player node_player =
                    std::invoke(player_to_move, node.state);
                for(std::size_t played = path_index;
                    played < played_moves.size(); ++played){
                    if(!(played_players[played] == node_player)) continue;
                    for(std::size_t move_index = 0;
                        move_index < node.moves.size(); ++move_index){
                        if(!(node.moves[move_index] == played_moves[played])) continue;
                        bool already_seen = false;
                        for(std::size_t earlier = path_index;
                            earlier < played; ++earlier){
                            if(played_players[earlier] == node_player
                               && node.moves[move_index]
                                    == played_moves[earlier]){
                                already_seen = true;
                                break;
                            }
                        }
                        if(!already_seen){
                            ++node.amaf[move_index].visits;
                            node.amaf[move_index].reward_sum += outcome;
                            ++amaf_updates;
                        }
                        break;
                    }
                }
            }
        }
        if(mode == deterministic_mode::solver){
            for(auto iterator = path.rbegin(); iterator != path.rend(); ++iterator){
                update_proven(nodes[*iterator]);
            }
        }
    }

    Result result;
    result.tree_nodes = nodes.size();
    result.amaf_updates = amaf_updates;
    result.proven_outcome = nodes[0].proven;
    if(nodes[0].children.empty()) return result;

    std::size_t selected = nodes[0].children.front();
    for(const std::size_t child : nodes[0].children){
        const auto proof_rank = [](MctsProvenOutcome outcome){
            switch(outcome){
                case MctsProvenOutcome::win: return 3;
                case MctsProvenOutcome::unknown: return 2;
                case MctsProvenOutcome::draw: return 1;
                case MctsProvenOutcome::loss: return 0;
            }
            return 0;
        };
        if(mode == deterministic_mode::solver){
            const int candidate_rank = proof_rank(nodes[child].proven);
            const int selected_rank = proof_rank(nodes[selected].proven);
            if(selected_rank < candidate_rank){
                selected = child;
                continue;
            }
            if(candidate_rank < selected_rank) continue;
        }
        if(nodes[selected].visits < nodes[child].visits){
            selected = child;
        }else if(nodes[selected].visits == nodes[child].visits
                 && nodes[selected].visits != 0){
            const double selected_mean = nodes[selected].reward_sum
                / static_cast<double>(nodes[selected].visits);
            const double child_mean = nodes[child].reward_sum
                / static_cast<double>(nodes[child].visits);
            if(selected_mean < child_mean) selected = child;
        }
    }
    result.move = nodes[selected].incoming_move;
    result.visits = nodes[selected].visits;
    if(result.visits != 0){
        result.average_reward = nodes[selected].reward_sum
            / static_cast<double>(result.visits);
    }
    return result;
}

} // namespace advanced_monte_carlo_tree_search_internal

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class Prior, class RandomEngine>
auto puct_monte_carlo_tree_search(
    const State& root, std::size_t iterations, Terminal terminal,
    LegalMoves legal_moves, Transition transition, PlayerToMove player_to_move,
    Reward reward, Prior prior, RandomEngine& random_engine,
    double exploration = 1.0, std::size_t maximum_rollout_depth = 256
){
    const auto unused_bias = [](const auto&, const auto&){ return 0.0; };
    return advanced_monte_carlo_tree_search_internal::deterministic_search(
        root, iterations, terminal, legal_moves, transition, player_to_move,
        reward, prior, unused_bias, random_engine,
        advanced_monte_carlo_tree_search_internal::deterministic_mode::puct,
        exploration, 0.0, {}, 0.0, maximum_rollout_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class RandomEngine>
auto rave_monte_carlo_tree_search(
    const State& root, std::size_t iterations, Terminal terminal,
    LegalMoves legal_moves, Transition transition, PlayerToMove player_to_move,
    Reward reward, RandomEngine& random_engine,
    double exploration = 1.4142135623730950488,
    double rave_equivalence = 0.001,
    std::size_t maximum_rollout_depth = 256
){
    const auto unused = [](const auto&, const auto&){ return 0.0; };
    return advanced_monte_carlo_tree_search_internal::deterministic_search(
        root, iterations, terminal, legal_moves, transition, player_to_move,
        reward, unused, unused, random_engine,
        advanced_monte_carlo_tree_search_internal::deterministic_mode::rave,
        exploration, rave_equivalence, {}, 0.0, maximum_rollout_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class RandomEngine>
auto amaf_monte_carlo_tree_search(
    const State& root, std::size_t iterations, Terminal terminal,
    LegalMoves legal_moves, Transition transition, PlayerToMove player_to_move,
    Reward reward, RandomEngine& random_engine,
    double exploration = 1.4142135623730950488,
    std::size_t maximum_rollout_depth = 256
){
    const auto unused = [](const auto&, const auto&){ return 0.0; };
    return advanced_monte_carlo_tree_search_internal::deterministic_search(
        root, iterations, terminal, legal_moves, transition, player_to_move,
        reward, unused, unused, random_engine,
        advanced_monte_carlo_tree_search_internal::deterministic_mode::amaf,
        exploration, 0.0, {}, 0.0, maximum_rollout_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class RandomEngine>
auto mcts_solver(
    const State& root, std::size_t iterations, Terminal terminal,
    LegalMoves legal_moves, Transition transition, PlayerToMove player_to_move,
    Reward reward, RandomEngine& random_engine,
    double exploration = 1.4142135623730950488,
    std::size_t maximum_rollout_depth = 256
){
    const auto unused = [](const auto&, const auto&){ return 0.0; };
    return advanced_monte_carlo_tree_search_internal::deterministic_search(
        root, iterations, terminal, legal_moves, transition, player_to_move,
        reward, unused, unused, random_engine,
        advanced_monte_carlo_tree_search_internal::deterministic_mode::solver,
        exploration, 0.0, {}, 0.0, maximum_rollout_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class RandomEngine>
auto progressive_widening_monte_carlo_tree_search(
    const State& root, std::size_t iterations, Terminal terminal,
    LegalMoves legal_moves, Transition transition, PlayerToMove player_to_move,
    Reward reward, RandomEngine& random_engine,
    MctsProgressiveWidening widening = {},
    double exploration = 1.4142135623730950488,
    std::size_t maximum_rollout_depth = 256
){
    const auto unused = [](const auto&, const auto&){ return 0.0; };
    return advanced_monte_carlo_tree_search_internal::deterministic_search(
        root, iterations, terminal, legal_moves, transition, player_to_move,
        reward, unused, unused, random_engine,
        advanced_monte_carlo_tree_search_internal::deterministic_mode::
            progressive_widening,
        exploration, 0.0, widening, 0.0, maximum_rollout_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class Bias, class RandomEngine>
auto progressive_bias_monte_carlo_tree_search(
    const State& root, std::size_t iterations, Terminal terminal,
    LegalMoves legal_moves, Transition transition, PlayerToMove player_to_move,
    Reward reward, Bias bias, RandomEngine& random_engine,
    double bias_weight = 1.0,
    double exploration = 1.4142135623730950488,
    std::size_t maximum_rollout_depth = 256
){
    const auto unused_prior = [](const auto&, const auto&){ return 0.0; };
    return advanced_monte_carlo_tree_search_internal::deterministic_search(
        root, iterations, terminal, legal_moves, transition, player_to_move,
        reward, unused_prior, bias, random_engine,
        advanced_monte_carlo_tree_search_internal::deterministic_mode::
            progressive_bias,
        exploration, 0.0, {}, bias_weight, maximum_rollout_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class Reward, class Player, class Policy, class RandomEngine>
auto monte_carlo_rollout_with_policy(
    const State& initial_state,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    Reward reward,
    const Player& reward_player,
    Policy policy,
    RandomEngine& random_engine,
    std::size_t maximum_depth = 256
){
    using MoveRange = std::invoke_result_t<LegalMoves&, const State&>;
    using Move =
        advanced_monte_carlo_tree_search_internal::range_value_t<MoveRange>;
    MonteCarloRolloutResult<State, Move> result{
        initial_state, {}, 0.0, false
    };
    for(std::size_t depth = 0; depth < maximum_depth; ++depth){
        if(static_cast<bool>(std::invoke(terminal, result.final_state))) break;
        auto moves =
            advanced_monte_carlo_tree_search_internal::collect_moves<Move>(
                std::invoke(legal_moves, result.final_state)
            );
        if(moves.empty()) break;
        const std::size_t selected = static_cast<std::size_t>(
            std::invoke(policy, result.final_state, moves, random_engine)
        );
        if(selected >= moves.size())[[unlikely]]{
            throw std::out_of_range(
                "rollout policy returned an invalid move index"
            );
        }
        result.moves.push_back(moves[selected]);
        result.final_state = std::invoke(
            transition, result.final_state, moves[selected]
        );
    }
    result.terminal =
        static_cast<bool>(std::invoke(terminal, result.final_state));
    result.reward = static_cast<double>(
        std::invoke(reward, result.final_state, reward_player)
    );
    if(!std::isfinite(result.reward))[[unlikely]]{
        throw std::invalid_argument("rollout reward must be finite");
    }
    return result;
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class Reward, class Player, class RandomEngine>
auto monte_carlo_rollout(
    const State& initial_state,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    Reward reward,
    const Player& reward_player,
    RandomEngine& random_engine,
    std::size_t maximum_depth = 256
){
    const auto uniform_policy = [](const auto&, const auto& moves, auto& engine){
        std::uniform_int_distribution<std::size_t> distribution(
            0, moves.size() - 1
        );
        return distribution(engine);
    };
    return monte_carlo_rollout_with_policy(
        initial_state, terminal, legal_moves, transition, reward,
        reward_player, uniform_policy, random_engine, maximum_depth
    );
}

template<class State, class Terminal, class LegalMoves,
         class StochasticTransition, class PlayerToMove, class Reward,
         class RandomEngine>
auto double_progressive_widening_monte_carlo_tree_search(
    const State& root,
    std::size_t iterations,
    Terminal terminal,
    LegalMoves legal_moves,
    StochasticTransition stochastic_transition,
    PlayerToMove player_to_move,
    Reward reward,
    RandomEngine& random_engine,
    MctsProgressiveWidening action_widening = {},
    MctsProgressiveWidening outcome_widening = {},
    double exploration = 1.4142135623730950488,
    std::size_t maximum_rollout_depth = 256
){
    using MoveRange = std::invoke_result_t<LegalMoves&, const State&>;
    using Move =
        advanced_monte_carlo_tree_search_internal::range_value_t<MoveRange>;
    using Player = std::decay_t<
        std::invoke_result_t<PlayerToMove&, const State&>
    >;
    using Result = AdvancedMonteCarloTreeSearchResult<Move>;

    struct Action{
        std::size_t move_index = 0;
        std::vector<std::size_t> outcomes;
        std::size_t visits = 0;
        double reward_sum = 0.0;
    };
    struct Node{
        State state;
        std::vector<Move> moves;
        std::vector<std::size_t> unexpanded;
        std::vector<Action> actions;
        std::size_t visits = 0;
        double reward_sum = 0.0;
    };

    advanced_monte_carlo_tree_search_internal::require_widening(
        action_widening
    );
    advanced_monte_carlo_tree_search_internal::require_widening(
        outcome_widening
    );
    advanced_monte_carlo_tree_search_internal::require_non_negative_finite(
        exploration, "double progressive widening requires finite exploration"
    );
    const Player root_player = std::invoke(player_to_move, root);
    const auto make_node = [&](State state){
        Node node{
            std::move(state), {}, {}, {}, 0, 0.0
        };
        node.moves =
            advanced_monte_carlo_tree_search_internal::collect_moves<Move>(
                std::invoke(legal_moves, node.state)
            );
        node.unexpanded.resize(node.moves.size());
        for(std::size_t index = 0; index < node.moves.size(); ++index){
            node.unexpanded[index] = index;
        }
        return node;
    };

    std::vector<Node> nodes;
    nodes.push_back(make_node(root));
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::size_t current = 0;
        std::vector<std::size_t> node_path{0};
        std::vector<std::pair<std::size_t, std::size_t>> action_path;

        while(!static_cast<bool>(std::invoke(terminal, nodes[current].state))){
            if(nodes[current].moves.empty()) break;
            std::size_t selected_action = 0;
            const std::size_t action_limit =
                advanced_monte_carlo_tree_search_internal::widening_limit(
                    action_widening, nodes[current].visits,
                    nodes[current].moves.size()
                );
            if(!nodes[current].unexpanded.empty()
               && nodes[current].actions.size() < action_limit){
                std::uniform_int_distribution<std::size_t> distribution(
                    0, nodes[current].unexpanded.size() - 1
                );
                const std::size_t position = distribution(random_engine);
                const std::size_t move_index =
                    nodes[current].unexpanded[position];
                if(position + 1 != nodes[current].unexpanded.size()){
                    nodes[current].unexpanded[position] =
                        nodes[current].unexpanded.back();
                }
                nodes[current].unexpanded.pop_back();
                selected_action = nodes[current].actions.size();
                nodes[current].actions.push_back(
                    Action{move_index, {}, 0, 0.0}
                );
            }else{
                const bool maximize =
                    std::invoke(player_to_move, nodes[current].state)
                    == root_player;
                double selected_score =
                    -std::numeric_limits<double>::infinity();
                for(std::size_t action = 0;
                    action < nodes[current].actions.size(); ++action){
                    const Action& candidate = nodes[current].actions[action];
                    const double mean = candidate.visits == 0 ? 0.0
                        : candidate.reward_sum
                            / static_cast<double>(candidate.visits);
                    const double score = (maximize ? mean : -mean)
                        + exploration * std::sqrt(
                            std::log(static_cast<double>(
                                std::max<std::size_t>(
                                    nodes[current].visits, 1
                                )
                            ) + 1.0)
                            / static_cast<double>(
                                std::max<std::size_t>(candidate.visits, 1)
                            )
                        );
                    if(selected_score < score){
                        selected_score = score;
                        selected_action = action;
                    }
                }
            }

            action_path.emplace_back(current, selected_action);
            Action& action = nodes[current].actions[selected_action];
            const std::size_t outcome_limit =
                advanced_monte_carlo_tree_search_internal::widening_limit(
                    outcome_widening, action.visits,
                    std::numeric_limits<std::size_t>::max()
                );
            if(action.outcomes.empty()
               || action.outcomes.size() < outcome_limit){
                const Move move = nodes[current].moves[action.move_index];
                State next = std::invoke(
                    stochastic_transition, nodes[current].state, move,
                    random_engine
                );
                const std::size_t child = nodes.size();
                nodes.push_back(make_node(std::move(next)));
                nodes[current].actions[selected_action].outcomes.push_back(child);
                current = child;
                node_path.push_back(current);
                break;
            }
            std::vector<double> weights;
            weights.reserve(action.outcomes.size());
            for(const std::size_t child : action.outcomes){
                weights.push_back(static_cast<double>(
                    std::max<std::size_t>(nodes[child].visits, 1)
                ));
            }
            std::discrete_distribution<std::size_t> distribution(
                weights.begin(), weights.end()
            );
            current = action.outcomes[distribution(random_engine)];
            node_path.push_back(current);
        }

        State rollout_state = nodes[current].state;
        for(std::size_t depth = 0; depth < maximum_rollout_depth; ++depth){
            if(static_cast<bool>(std::invoke(terminal, rollout_state))) break;
            auto moves =
                advanced_monte_carlo_tree_search_internal::collect_moves<Move>(
                    std::invoke(legal_moves, rollout_state)
                );
            if(moves.empty()) break;
            std::uniform_int_distribution<std::size_t> distribution(
                0, moves.size() - 1
            );
            const Move move = moves[distribution(random_engine)];
            rollout_state = std::invoke(
                stochastic_transition, rollout_state, move, random_engine
            );
        }
        const double outcome = static_cast<double>(
            std::invoke(reward, rollout_state, root_player)
        );
        if(!std::isfinite(outcome))[[unlikely]]{
            throw std::invalid_argument("DPW reward must be finite");
        }
        for(const std::size_t node : node_path){
            ++nodes[node].visits;
            nodes[node].reward_sum += outcome;
        }
        for(const auto [node, action] : action_path){
            ++nodes[node].actions[action].visits;
            nodes[node].actions[action].reward_sum += outcome;
        }
    }

    Result result;
    result.tree_nodes = nodes.size();
    if(nodes[0].actions.empty()) return result;
    std::size_t selected = 0;
    for(std::size_t action = 1; action < nodes[0].actions.size(); ++action){
        const Action& best = nodes[0].actions[selected];
        const Action& candidate = nodes[0].actions[action];
        if(best.visits < candidate.visits
           || (best.visits == candidate.visits && best.visits != 0
               && best.reward_sum / static_cast<double>(best.visits)
                    < candidate.reward_sum
                        / static_cast<double>(candidate.visits))){
            selected = action;
        }
    }
    const Action& best = nodes[0].actions[selected];
    result.move = nodes[0].moves[best.move_index];
    result.visits = best.visits;
    if(best.visits != 0){
        result.average_reward =
            best.reward_sum / static_cast<double>(best.visits);
    }
    return result;
}

namespace advanced_monte_carlo_tree_search_internal{

template<class State, class Terminal, class LegalMoves, class Transition,
         class Score, class RandomEngine>
auto nested_monte_carlo_impl(
    const State& initial_state,
    std::size_t level,
    Terminal& terminal,
    LegalMoves& legal_moves,
    Transition& transition,
    Score& score,
    RandomEngine& random_engine,
    std::size_t maximum_depth
){
    using MoveRange = std::invoke_result_t<LegalMoves&, const State&>;
    using Move = range_value_t<MoveRange>;
    using Result = NestedMonteCarloSearchResult<Move>;

    if(level == 0){
        State state = initial_state;
        Result result;
        for(std::size_t depth = 0; depth < maximum_depth; ++depth){
            if(static_cast<bool>(std::invoke(terminal, state))) break;
            auto moves = collect_moves<Move>(std::invoke(legal_moves, state));
            if(moves.empty()) break;
            std::uniform_int_distribution<std::size_t> distribution(
                0, moves.size() - 1
            );
            const Move move = moves[distribution(random_engine)];
            result.moves.push_back(move);
            state = std::invoke(transition, state, move);
        }
        result.score = static_cast<double>(std::invoke(score, state));
        if(!std::isfinite(result.score))[[unlikely]]{
            throw std::invalid_argument("NMCS score must be finite");
        }
        result.rollouts = 1;
        return result;
    }

    State state = initial_state;
    Result best;
    std::vector<Move> prefix;
    std::size_t total_rollouts = 0;
    for(std::size_t depth = 0; depth < maximum_depth; ++depth){
        if(static_cast<bool>(std::invoke(terminal, state))) break;
        auto moves = collect_moves<Move>(std::invoke(legal_moves, state));
        if(moves.empty()) break;
        Result selected;
        std::optional<Move> selected_move;
        for(const Move& move : moves){
            State next = std::invoke(transition, state, move);
            Result candidate = nested_monte_carlo_impl(
                next, level - 1, terminal, legal_moves, transition,
                score, random_engine, maximum_depth - depth - 1
            );
            total_rollouts += candidate.rollouts;
            candidate.moves.insert(candidate.moves.begin(), move);
            if(!selected_move || selected.score < candidate.score){
                selected = std::move(candidate);
                selected_move = move;
            }
        }
        std::vector<Move> complete = prefix;
        complete.insert(
            complete.end(), selected.moves.begin(), selected.moves.end()
        );
        if(best.moves.empty() || best.score < selected.score){
            best.moves = std::move(complete);
            best.score = selected.score;
        }
        prefix.push_back(*selected_move);
        state = std::invoke(transition, state, *selected_move);
    }
    if(best.moves.empty()){
        best.moves = std::move(prefix);
        best.score = static_cast<double>(std::invoke(score, state));
        if(!std::isfinite(best.score))[[unlikely]]{
            throw std::invalid_argument("NMCS score must be finite");
        }
    }
    best.rollouts = total_rollouts;
    return best;
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class Score, class PolicyIndex, class RandomEngine>
auto nrpa_impl(
    const State& root,
    std::size_t level,
    std::size_t iterations_per_level,
    Terminal& terminal,
    LegalMoves& legal_moves,
    Transition& transition,
    Score& score,
    PolicyIndex& policy_index,
    const std::vector<double>& input_policy,
    double learning_rate,
    RandomEngine& random_engine,
    std::size_t maximum_depth
){
    using MoveRange = std::invoke_result_t<LegalMoves&, const State&>;
    using Move = range_value_t<MoveRange>;
    using Result = NrpaSearchResult<Move>;

    const auto logits = [&](const State& state, const std::vector<Move>& moves,
                            const std::vector<double>& policy){
        std::vector<std::size_t> indices;
        std::vector<double> weights;
        indices.reserve(moves.size());
        weights.reserve(moves.size());
        double maximum = -std::numeric_limits<double>::infinity();
        for(const Move& move : moves){
            const std::size_t index = static_cast<std::size_t>(
                std::invoke(policy_index, state, move)
            );
            if(index >= policy.size())[[unlikely]]{
                throw std::out_of_range("NRPA policy index is out of range");
            }
            indices.push_back(index);
            maximum = std::max(maximum, policy[index]);
        }
        for(const std::size_t index : indices){
            weights.push_back(std::exp(policy[index] - maximum));
        }
        return std::pair{std::move(indices), std::move(weights)};
    };

    if(level == 0){
        Result result;
        result.policy = input_policy;
        State state = root;
        for(std::size_t depth = 0; depth < maximum_depth; ++depth){
            if(static_cast<bool>(std::invoke(terminal, state))) break;
            auto moves = collect_moves<Move>(std::invoke(legal_moves, state));
            if(moves.empty()) break;
            auto [indices, weights] = logits(state, moves, input_policy);
            static_cast<void>(indices);
            std::discrete_distribution<std::size_t> distribution(
                weights.begin(), weights.end()
            );
            const std::size_t selected = distribution(random_engine);
            result.moves.push_back(moves[selected]);
            state = std::invoke(transition, state, moves[selected]);
        }
        result.score = static_cast<double>(std::invoke(score, state));
        if(!std::isfinite(result.score))[[unlikely]]{
            throw std::invalid_argument("NRPA score must be finite");
        }
        result.rollouts = 1;
        return result;
    }

    std::vector<double> policy = input_policy;
    Result best;
    std::size_t rollouts = 0;
    for(std::size_t iteration = 0; iteration < iterations_per_level;
        ++iteration){
        Result candidate = nrpa_impl(
            root, level - 1, iterations_per_level, terminal, legal_moves,
            transition, score, policy_index, policy, learning_rate,
            random_engine, maximum_depth
        );
        rollouts += candidate.rollouts;
        if(best.moves.empty() || best.score < candidate.score){
            best = std::move(candidate);
        }

        State state = root;
        for(const Move& chosen : best.moves){
            if(static_cast<bool>(std::invoke(terminal, state))) break;
            auto moves = collect_moves<Move>(std::invoke(legal_moves, state));
            if(moves.empty()) break;
            auto [indices, weights] = logits(state, moves, policy);
            double sum = 0.0;
            for(const double weight : weights) sum += weight;
            std::optional<std::size_t> chosen_position;
            for(std::size_t index = 0; index < moves.size(); ++index){
                policy[indices[index]] -=
                    learning_rate * weights[index] / sum;
                if(!chosen_position && moves[index] == chosen){
                    chosen_position = index;
                }
            }
            if(!chosen_position)[[unlikely]]{
                throw std::logic_error(
                    "NRPA best sequence is not legal during adaptation"
                );
            }
            policy[indices[*chosen_position]] += learning_rate;
            state = std::invoke(
                transition, state, moves[*chosen_position]
            );
        }
    }
    best.policy = std::move(policy);
    best.rollouts = rollouts;
    return best;
}

} // namespace advanced_monte_carlo_tree_search_internal

template<class State, class Terminal, class LegalMoves, class Transition,
         class Score, class RandomEngine>
auto nested_monte_carlo_search(
    const State& root,
    std::size_t level,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    Score score,
    RandomEngine& random_engine,
    std::size_t maximum_depth = 256
){
    return advanced_monte_carlo_tree_search_internal::nested_monte_carlo_impl(
        root, level, terminal, legal_moves, transition, score,
        random_engine, maximum_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class Score, class PolicyIndex, class RandomEngine>
auto nested_rollout_policy_adaptation(
    const State& root,
    std::size_t level,
    std::size_t iterations_per_level,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    Score score,
    PolicyIndex policy_index,
    std::size_t policy_size,
    RandomEngine& random_engine,
    double learning_rate = 1.0,
    std::size_t maximum_depth = 256
){
    if(iterations_per_level == 0 && level != 0)[[unlikely]]{
        throw std::invalid_argument(
            "NRPA iterations_per_level must be positive"
        );
    }
    if(policy_size == 0)[[unlikely]]{
        throw std::invalid_argument("NRPA policy_size must be positive");
    }
    advanced_monte_carlo_tree_search_internal::require_non_negative_finite(
        learning_rate, "NRPA learning rate must be non-negative and finite"
    );
    return advanced_monte_carlo_tree_search_internal::nrpa_impl(
        root, level, iterations_per_level, terminal, legal_moves, transition,
        score, policy_index, std::vector<double>(policy_size), learning_rate,
        random_engine, maximum_depth
    );
}

template<class State, class Terminal, class LegalMoves, class Transition,
         class Score, class PolicyIndex, class RandomEngine>
auto nrpa(
    const State& root,
    std::size_t level,
    std::size_t iterations_per_level,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    Score score,
    PolicyIndex policy_index,
    std::size_t policy_size,
    RandomEngine& random_engine,
    double learning_rate = 1.0,
    std::size_t maximum_depth = 256
){
    return nested_rollout_policy_adaptation(
        root, level, iterations_per_level, terminal, legal_moves, transition,
        score, policy_index, policy_size, random_engine, learning_rate,
        maximum_depth
    );
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ADVANCED_MONTE_CARLO_TREE_SEARCH_HPP_INCLUDED
