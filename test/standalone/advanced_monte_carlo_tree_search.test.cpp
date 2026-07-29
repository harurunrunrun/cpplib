// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include "../../src/approximate/search/advanced_monte_carlo_tree_search.hpp"
#include "../../src/approximate/search/monte_carlo_tree_search.hpp"
#include "../../src/approximate/search/multi_armed_bandit.hpp"

struct OneStepState{
    int selected = -1;
};

struct SequenceState{
    int depth = 0;
    int value = 0;
};

void fixed_tests(){
    const std::vector<double> values{-2.0, 1.0, 7.0, 3.0};
    const auto terminal = [](const OneStepState& state){
        return state.selected >= 0;
    };
    const auto legal_moves = [&](const OneStepState& state){
        std::vector<int> moves;
        if(state.selected < 0){
            for(std::size_t index = 0; index < values.size(); ++index){
                moves.push_back(static_cast<int>(index));
            }
        }
        return moves;
    };
    const auto transition = [](const OneStepState&, int move){
        return OneStepState{move};
    };
    const auto player = [](const OneStepState& state){
        return state.selected < 0 ? 0 : 1;
    };
    const auto reward = [&](const OneStepState& state, int){
        return values[static_cast<std::size_t>(state.selected)];
    };
    const auto prior = [](const OneStepState&, int move){
        return move == 2 ? 100.0 : 1.0;
    };
    std::mt19937 engine(20260729U);
    const auto puct = puct_monte_carlo_tree_search(
        OneStepState{}, 1, terminal, legal_moves, transition, player,
        reward, prior, engine, 1.0, 1
    );
    assert(puct.move && *puct.move == 2);

    engine.seed(1);
    const auto rave = rave_monte_carlo_tree_search(
        OneStepState{}, 400, terminal, legal_moves, transition, player,
        reward, engine, 0.7, 0.01, 1
    );
    assert(rave.move && *rave.move == 2);
    assert(rave.amaf_updates != 0);
    engine.seed(2);
    const auto amaf = amaf_monte_carlo_tree_search(
        OneStepState{}, 400, terminal, legal_moves, transition, player,
        reward, engine, 0.7, 1
    );
    assert(amaf.move && *amaf.move == 2);
    assert(amaf.amaf_updates != 0);

    const std::vector<double> solved_values{-1.0, 0.0, 1.0};
    const auto solved_moves = [&](const OneStepState& state){
        std::vector<int> moves;
        if(state.selected < 0) moves = {0, 1, 2};
        return moves;
    };
    const auto solved_reward = [&](const OneStepState& state, int){
        return solved_values[static_cast<std::size_t>(state.selected)];
    };
    engine.seed(3);
    const auto solved = mcts_solver(
        OneStepState{}, 100, terminal, solved_moves, transition, player,
        solved_reward, engine, 1.0, 1
    );
    assert(solved.move && *solved.move == 2);
    assert(solved.proven_outcome == MctsProvenOutcome::win);
    assert(solved.tree_nodes <= 4);

    std::vector<double> many_values(100);
    for(std::size_t index = 0; index < many_values.size(); ++index){
        many_values[index] = static_cast<double>(index);
    }
    const auto many_moves = [&](const OneStepState& state){
        std::vector<int> moves;
        if(state.selected < 0){
            for(int move = 0; move < 100; ++move) moves.push_back(move);
        }
        return moves;
    };
    const auto many_reward = [&](const OneStepState& state, int){
        return many_values[static_cast<std::size_t>(state.selected)];
    };
    engine.seed(4);
    const auto widened = progressive_widening_monte_carlo_tree_search(
        OneStepState{}, 16, terminal, many_moves, transition, player,
        many_reward, engine, MctsProgressiveWidening{1.0, 0.5}, 0.5, 1
    );
    assert(widened.tree_nodes == 4);

    const auto bias = [](const OneStepState&, int move){
        return move == 2 ? 10.0 : 0.0;
    };
    engine.seed(5);
    const auto biased = progressive_bias_monte_carlo_tree_search(
        OneStepState{}, 300, terminal, legal_moves, transition, player,
        reward, bias, engine, 10.0, 0.2, 1
    );
    assert(biased.move && *biased.move == 2);

    const auto last_policy = [](const OneStepState&, const std::vector<int>& moves,
                                std::mt19937&){
        return moves.size() - 1;
    };
    const auto rollout = monte_carlo_rollout_with_policy(
        OneStepState{}, terminal, legal_moves, transition, reward, 0,
        last_policy, engine, 1
    );
    assert(rollout.terminal);
    assert(rollout.moves == std::vector<int>{3});
    assert(rollout.reward == 3.0);
    const auto uniform_rollout = monte_carlo_rollout(
        OneStepState{}, terminal, legal_moves, transition, reward, 0,
        engine, 1
    );
    assert(uniform_rollout.moves.size() == 1);

    const auto stochastic_transition = [](
        const OneStepState&, int move, std::mt19937& generator
    ){
        if(move == 0){
            std::bernoulli_distribution noisy(0.2);
            return OneStepState{noisy(generator) ? 2 : 0};
        }
        return OneStepState{2};
    };
    const auto stochastic_reward = [](const OneStepState& state, int){
        return state.selected == 2 ? 1.0 : 0.0;
    };
    const auto two_moves = [](const OneStepState& state){
        return state.selected < 0
            ? std::vector<int>{0, 1} : std::vector<int>{};
    };
    engine.seed(6);
    const auto dpw = double_progressive_widening_monte_carlo_tree_search(
        OneStepState{}, 1000, terminal, two_moves, stochastic_transition,
        player, stochastic_reward, engine,
        MctsProgressiveWidening{1.0, 0.5},
        MctsProgressiveWidening{1.0, 0.5}, 0.7, 1
    );
    assert(dpw.move && *dpw.move == 1);
    assert(dpw.tree_nodes < 100);

    const auto sequence_terminal = [](const SequenceState& state){
        return state.depth == 4;
    };
    const auto sequence_moves = [](const SequenceState& state){
        return state.depth < 4
            ? std::vector<int>{0, 1, 2} : std::vector<int>{};
    };
    const auto sequence_transition = [](const SequenceState& state, int move){
        return SequenceState{state.depth + 1, state.value * 3 + move};
    };
    const auto sequence_score = [](const SequenceState& state){
        return -std::abs(80 - state.value);
    };
    engine.seed(7);
    const auto nmcs = nested_monte_carlo_search(
        SequenceState{}, 2, sequence_terminal, sequence_moves,
        sequence_transition, sequence_score, engine, 4
    );
    assert(nmcs.score == 0.0);
    assert(nmcs.moves == std::vector<int>({2, 2, 2, 2}));
    assert(nmcs.rollouts > 1);

    const auto policy_index = [](const SequenceState& state, int move){
        return static_cast<std::size_t>(state.depth * 3 + move);
    };
    engine.seed(8);
    const auto adapted = nested_rollout_policy_adaptation(
        SequenceState{}, 2, 20, sequence_terminal, sequence_moves,
        sequence_transition, sequence_score, policy_index, 12, engine,
        1.0, 4
    );
    assert(adapted.score == 0.0);
    assert(adapted.rollouts == 400);
    assert(std::any_of(
        adapted.policy.begin(), adapted.policy.end(),
        [](double weight){ return std::abs(weight) > 1e-12; }
    ));
    engine.seed(8);
    const auto alias = nrpa(
        SequenceState{}, 1, 20, sequence_terminal, sequence_moves,
        sequence_transition, sequence_score, policy_index, 12, engine,
        1.0, 4
    );
    assert(alias.rollouts == 20);

    Ucb1Bandit<int> bandit(1);
    bandit.update(0, 1);
    assert(bandit.select_arm() == 0);
}

int main(){
    fixed_tests();
    int count;
    std::size_t iterations;
    if(!(std::cin >> count >> iterations)) return 0;
    std::vector<double> values(static_cast<std::size_t>(count));
    for(double& value : values) std::cin >> value;
    const auto terminal = [](const OneStepState& state){
        return state.selected >= 0;
    };
    const auto legal_moves = [count](const OneStepState& state){
        std::vector<int> moves;
        if(state.selected < 0){
            for(int move = 0; move < count; ++move) moves.push_back(move);
        }
        return moves;
    };
    const auto transition = [](const OneStepState&, int move){
        return OneStepState{move};
    };
    const auto player = [](const OneStepState& state){
        return state.selected < 0 ? 0 : 1;
    };
    const auto reward = [&](const OneStepState& state, int){
        return values[static_cast<std::size_t>(state.selected)];
    };
    const auto prior = [&](const OneStepState&, int move){
        const double minimum = *std::min_element(values.begin(), values.end());
        return values[static_cast<std::size_t>(move)] - minimum + 1.0;
    };
    std::mt19937 engine(123456789U);
    const auto result = puct_monte_carlo_tree_search(
        OneStepState{}, iterations, terminal, legal_moves, transition, player,
        reward, prior, engine, 0.7, 1
    );
    std::cout << result.move.value_or(-1) << '\n';
}
