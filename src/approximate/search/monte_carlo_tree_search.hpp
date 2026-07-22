#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_MONTE_CARLO_TREE_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_MONTE_CARLO_TREE_SEARCH_HPP_INCLUDED

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

template<class Move>
struct MonteCarloTreeSearchResult{
    std::optional<Move> move;
    std::size_t visits = 0;
    double average_reward = 0.0;
    std::size_t tree_nodes = 0;
};

namespace monte_carlo_tree_search_internal{

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

} // namespace monte_carlo_tree_search_internal

template<class State, class Terminal, class LegalMoves, class Transition,
         class PlayerToMove, class Reward, class RandomEngine>
auto monte_carlo_tree_search(
    const State& root,
    std::size_t iterations,
    Terminal terminal,
    LegalMoves legal_moves,
    Transition transition,
    PlayerToMove player_to_move,
    Reward reward,
    RandomEngine& random_engine,
    double exploration = 1.4142135623730950488,
    std::size_t maximum_rollout_depth = 256
){
    using MoveRange = std::invoke_result_t<LegalMoves&, const State&>;
    using Move = monte_carlo_tree_search_internal::range_value_t<MoveRange>;
    using Player = std::decay_t<std::invoke_result_t<PlayerToMove&, const State&>>;
    using Result = MonteCarloTreeSearchResult<Move>;

    struct Node{
        State state;
        std::optional<Move> move;
        std::size_t parent;
        std::vector<std::size_t> children;
        std::vector<Move> unexpanded;
        std::size_t visits = 0;
        double reward_sum = 0.0;
    };

    if(exploration < 0.0 || !std::isfinite(exploration))[[unlikely]]{
        throw std::invalid_argument("monte_carlo_tree_search requires non-negative exploration");
    }

    const Player root_player = std::invoke(player_to_move, root);
    auto root_moves = monte_carlo_tree_search_internal::collect_moves<Move>(
        std::invoke(legal_moves, root)
    );
    std::vector<Node> nodes;
    nodes.push_back(Node{
        root, std::nullopt, 0, {}, std::move(root_moves), 0, 0.0
    });

    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        std::size_t current = 0;

        while(!static_cast<bool>(std::invoke(terminal, nodes[current].state))){
            if(!nodes[current].unexpanded.empty()){
                std::uniform_int_distribution<std::size_t> distribution(
                    0, nodes[current].unexpanded.size() - 1
                );
                const std::size_t position = distribution(random_engine);
                Move move = std::move(nodes[current].unexpanded[position]);
                if(position + 1 != nodes[current].unexpanded.size()){
                    nodes[current].unexpanded[position]
                        = std::move(nodes[current].unexpanded.back());
                }
                nodes[current].unexpanded.pop_back();

                State next_state = std::invoke(transition, nodes[current].state, move);
                auto next_moves = monte_carlo_tree_search_internal::collect_moves<Move>(
                    std::invoke(legal_moves, next_state)
                );
                const std::size_t child = nodes.size();
                nodes.push_back(Node{
                    std::move(next_state), std::move(move), current,
                    {}, std::move(next_moves), 0, 0.0
                });
                nodes[current].children.push_back(child);
                current = child;
                break;
            }
            if(nodes[current].children.empty()) break;

            const bool maximize_root_reward
                = std::invoke(player_to_move, nodes[current].state) == root_player;
            const double logarithm = std::log(static_cast<double>(nodes[current].visits));
            std::size_t selected = nodes[current].children.front();
            double selected_value = -std::numeric_limits<double>::infinity();
            for(const std::size_t child : nodes[current].children){
                const double mean = nodes[child].reward_sum
                    / static_cast<double>(nodes[child].visits);
                const double exploitation = maximize_root_reward ? mean : -mean;
                const double bonus = exploration * std::sqrt(
                    logarithm / static_cast<double>(nodes[child].visits)
                );
                const double value = exploitation + bonus;
                if(selected_value < value){
                    selected_value = value;
                    selected = child;
                }
            }
            current = selected;
        }

        State rollout_state = nodes[current].state;
        for(std::size_t depth = 0; depth < maximum_rollout_depth; ++depth){
            if(static_cast<bool>(std::invoke(terminal, rollout_state))) break;
            auto moves = monte_carlo_tree_search_internal::collect_moves<Move>(
                std::invoke(legal_moves, rollout_state)
            );
            if(moves.empty()) break;
            std::uniform_int_distribution<std::size_t> distribution(0, moves.size() - 1);
            const std::size_t position = distribution(random_engine);
            rollout_state = std::invoke(transition, rollout_state, moves[position]);
        }
        const double outcome = static_cast<double>(
            std::invoke(reward, rollout_state, root_player)
        );

        while(true){
            ++nodes[current].visits;
            nodes[current].reward_sum += outcome;
            if(current == 0) break;
            current = nodes[current].parent;
        }
    }

    Result result;
    result.tree_nodes = nodes.size();
    if(nodes[0].children.empty()) return result;

    std::size_t selected = nodes[0].children.front();
    for(const std::size_t child : nodes[0].children){
        if(nodes[selected].visits < nodes[child].visits){
            selected = child;
        }else if(nodes[selected].visits == nodes[child].visits
                 && nodes[selected].visits != 0
                 && nodes[selected].reward_sum / static_cast<double>(nodes[selected].visits)
                    < nodes[child].reward_sum / static_cast<double>(nodes[child].visits)){
            selected = child;
        }
    }
    result.move = nodes[selected].move;
    result.visits = nodes[selected].visits;
    if(result.visits != 0){
        result.average_reward = nodes[selected].reward_sum
                              / static_cast<double>(result.visits);
    }
    return result;
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_MONTE_CARLO_TREE_SEARCH_HPP_INCLUDED
