// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include "../../src/approximate/dp/advanced_knapsack.hpp"
#include "../../src/approximate/dp/approximate_state_dp.hpp"
#include "../../src/approximate/graph/advanced_coloring.hpp"
#include "../../src/approximate/graph/allocation.hpp"
#include "../../src/approximate/graph/semidefinite_max_cut.hpp"

namespace {

using State = std::pair<int, int>;

struct StateHash {
    [[nodiscard]] std::size_t operator()(const State& state) const noexcept {
        return static_cast<std::size_t>(
            static_cast<std::uint64_t>(
                static_cast<std::uint32_t>(state.first)
            ) << 32U
        ) ^ static_cast<std::uint32_t>(state.second);
    }
};

bool proper_coloring(
    const approximate::graph::ColoringResult& coloring,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    for(const auto [left, right] : edges){
        if(coloring.colors[left] == coloring.colors[right]) return false;
    }
    return true;
}

template<class Result>
bool feasible_knapsack(
    const Result& result,
    const std::vector<int>& weights,
    std::uint64_t capacity
){
    std::uint64_t sum = 0;
    std::set<std::size_t> seen;
    for(const std::size_t index : result.selected_indices){
        if(index >= weights.size() || !seen.insert(index).second) return false;
        sum += static_cast<std::uint64_t>(weights[index]);
    }
    return sum <= capacity && sum == result.total_weight;
}

void run_graph_and_allocation(std::mt19937_64& random){
    using namespace approximate::graph;
    const std::vector<std::pair<std::size_t, std::size_t>> cycle{
        {0, 1}, {1, 2}, {2, 3}, {3, 0}
    };
    const std::vector<std::vector<double>> vectors{
        {1.0, 0.0}, {-1.0, 0.0}, {1.0, 0.0}, {-1.0, 0.0}
    };
    const std::vector<std::array<double, 2>> directions{
        {1.0, 0.0}, {0.0, 1.0}
    };
    const auto rounded =
        semidefinite_rounding(vectors, cycle, directions);
    const auto randomized =
        goemans_williamson_max_cut(vectors, cycle, 8, random);
    assert(rounded.cut_edges == 4);
    assert(randomized.cut_edges == 4);

    const auto order = saturation_degree_ordering(4, cycle);
    std::set<std::size_t> order_set(order.begin(), order.end());
    assert(order.size() == 4 && order_set.size() == 4);
    const auto independent = independent_set_coloring(4, cycle);
    assert(independent.color_count == 2);
    assert(proper_coloring(independent, cycle));

    const std::vector<std::vector<double>> values{
        {10.0, 2.0, 3.0, 4.0, 1.0},
        {1.0, 11.0, 2.0, 3.0, 7.0},
        {2.0, 1.0, 12.0, 8.0, 6.0}
    };
    const auto auction = auction_algorithm(values, 0.01);
    const auto scaling =
        epsilon_scaling_auction_algorithm(values, 1.0, 0.01, 10.0);
    for(const auto& result : {auction, scaling}){
        assert(result.bidder_to_item.size() == values.size());
        std::set<std::size_t> assigned(
            result.bidder_to_item.begin(), result.bidder_to_item.end()
        );
        assert(assigned.size() == values.size());
    }
    const auto serial = random_serial_dictatorship(values, random);
    const auto round_robin =
        round_robin_allocation(values, std::vector<std::size_t>{2, 0, 1});
    const auto envy_free = envy_cycle_elimination(values);
    assert(serial.item_owner.size() == values.front().size());
    assert(round_robin.item_owner.size() == values.front().size());
    assert(envy_free.item_owner.size() == values.front().size());
    for(const std::size_t owner : round_robin.item_owner){
        assert(owner < values.size());
    }
    for(const std::size_t owner : envy_free.item_owner){
        assert(owner < values.size());
    }
}

void run_knapsack(){
    using namespace approximate::dp;
    const std::vector<int> weights{2, 3, 4, 5, 0};
    const std::vector<int> values{3, 4, 5, 8, 2};
    constexpr std::uint64_t capacity = 7;
    const auto core =
        core_problem_knapsack(weights, values, capacity, 5);
    const auto scaled =
        weight_scaling_knapsack_dp(weights, values, capacity, 4);
    const auto lagrangian =
        lagrangian_relaxation_knapsack(weights, values, capacity, 0.25);
    const auto dantzig =
        dantzig_bound_knapsack(weights, values, capacity);
    assert(feasible_knapsack(core, weights, capacity));
    assert(feasible_knapsack(scaled, weights, capacity));
    assert(feasible_knapsack(lagrangian, weights, capacity));
    assert(feasible_knapsack(dantzig.solution, weights, capacity));
    assert(dantzig.fractional_upper_bound + 1e-12L
           >= static_cast<long double>(dantzig.solution.total_value));

    const std::vector<std::vector<int>> constraints{
        {2, 3, 4, 5, 0},
        {3, 2, 2, 4, 0}
    };
    const auto surrogate = surrogate_relaxation_knapsack(
        constraints,
        values,
        std::vector<int>{7, 6},
        std::vector<double>{1.0, 0.5}
    );
    assert(surrogate.total_weights.size() == 2);
    assert(surrogate.total_weights[0] <= 7);
    assert(surrogate.total_weights[1] <= 6);
}

void run_state_dp(std::mt19937_64& random){
    using namespace approximate::dp;
    const auto expand = [](const State& state, std::size_t){
        return std::vector<State>{
            {state.first + 1, state.second},
            {state.first, state.second + 1},
            {state.first + 1, state.second}
        };
    };
    const auto score = [](const State& state){
        return state.first + state.second;
    };
    const auto key = [](const State& state){
        return state.first - state.second;
    };
    const auto objectives = [](const State& state){
        return std::array<long double, 2>{
            static_cast<long double>(state.first + 1),
            static_cast<long double>(state.second + 1)
        };
    };

    const auto beam = beam_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, score, 3
    );
    const auto top = top_k_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, score, 3
    );
    const auto sparse = sparse_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, key, score
    );
    const auto hash_beam = hash_beam_dynamic_programming(
        std::vector<State>{{0, 0}},
        4,
        expand,
        score,
        7,
        3,
        StateHash{}
    );
    const auto limited = state_count_limited_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, 3
    );
    const auto rounded = value_rounding_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, score, 2.0L
    );
    const auto compressed = state_compression_dynamic_programming(
        std::vector<State>{{0, 0}},
        4,
        expand,
        [](State state){
            state.first = std::min(state.first, 3);
            state.second = std::min(state.second, 3);
            return state;
        },
        StateHash{}
    );
    const auto aggregated = state_aggregation_dynamic_programming(
        std::vector<State>{{0, 0}},
        4,
        expand,
        key,
        [score](State left, State right){
            return score(left) >= score(right) ? left : right;
        }
    );
    const auto dominates = [](const State& left, const State& right){
        return left.first >= right.first && left.second >= right.second;
    };
    const auto pruned = dominance_pruned_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, dominates
    );
    const auto epsilon = epsilon_dominance_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, objectives, 0.25L
    );
    const auto pareto = approximate_pareto_frontier(
        std::vector<State>{{1, 4}, {2, 3}, {4, 1}, {1, 1}},
        objectives,
        0.1L
    );
    const auto interrupted = interrupted_dynamic_programming(
        std::vector<State>{{0, 0}},
        5,
        expand,
        [](std::size_t layer, const std::vector<State>&){
            return layer == 2;
        }
    );
    const auto stochastic = stochastic_beam_dynamic_programming(
        std::vector<State>{{0, 0}}, 4, expand, 3, random
    );

    assert(beam.states.size() <= 3 && beam.layers_completed == 4);
    assert(top.states.size() <= 3 && top.layers_completed == 4);
    assert(!sparse.states.empty() && sparse.layers_completed == 4);
    assert(hash_beam.states.size() <= 3);
    assert(limited.states.size() <= 3);
    assert(!rounded.states.empty());
    assert(!compressed.states.empty());
    assert(!aggregated.states.empty());
    assert(!pruned.states.empty());
    assert(!epsilon.states.empty());
    assert(pareto.size() == 3);
    assert(interrupted.interrupted && interrupted.layers_completed == 2);
    assert(stochastic.states.size() <= 3);
}

template<class Function>
void expect_exception(Function function){
    bool threw = false;
    try{
        function();
    }catch(const std::exception&){
        threw = true;
    }
    assert(threw);
}

void edge_case_tests(){
    using namespace approximate;
    expect_exception([]{
        static_cast<void>(graph::semidefinite_rounding(
            std::vector<std::vector<double>>{{1.0, 0.0}},
            std::vector<std::pair<std::size_t, std::size_t>>{},
            std::vector<std::vector<double>>{{1.0}}
        ));
    });
    expect_exception([]{
        static_cast<void>(graph::semidefinite_rounding(
            std::vector<std::vector<double>>{},
            std::vector<std::pair<std::size_t, std::size_t>>{{0, 0}},
            std::vector<std::vector<double>>{}
        ));
    });
    expect_exception([]{
        static_cast<void>(graph::auction_algorithm(
            std::vector<std::vector<double>>{{1.0}, {2.0}},
            0.1
        ));
    });
    expect_exception([]{
        static_cast<void>(graph::round_robin_allocation(
            std::vector<std::vector<int>>{{1}},
            std::vector<std::size_t>{}
        ));
    });
    expect_exception([]{
        static_cast<void>(graph::envy_cycle_elimination(
            std::vector<std::vector<int>>{{-1}}
        ));
    });
    expect_exception([]{
        static_cast<void>(graph::saturation_degree_ordering(
            1, std::vector<std::pair<std::size_t, std::size_t>>{{0, 0}}
        ));
    });
    expect_exception([]{
        static_cast<void>(dp::core_problem_knapsack(
            std::vector<int>{1},
            std::vector<int>{1},
            1,
            25
        ));
    });
    expect_exception([]{
        static_cast<void>(dp::weight_scaling_knapsack_dp(
            std::vector<int>{1},
            std::vector<int>{1},
            1,
            0
        ));
    });
    expect_exception([]{
        static_cast<void>(dp::surrogate_relaxation_knapsack(
            std::vector<std::vector<int>>{{1}},
            std::vector<int>{1},
            std::vector<int>{1},
            std::vector<double>{0.0}
        ));
    });
    const auto expand = [](const int& state, std::size_t){
        return std::vector<int>{state + 1};
    };
    expect_exception([&]{
        static_cast<void>(dp::beam_dynamic_programming(
            std::vector<int>{0}, 1, expand,
            [](const int& value){ return value; }, 0
        ));
    });
    expect_exception([&]{
        static_cast<void>(dp::value_rounding_dynamic_programming(
            std::vector<int>{0}, 1, expand,
            [](const int& value){ return value; }, 0.0L
        ));
    });
    expect_exception([&]{
        static_cast<void>(dp::epsilon_dominance_dynamic_programming(
            std::vector<int>{0},
            1,
            expand,
            [](const int& value){
                return std::array<long double, 1>{
                    static_cast<long double>(value)
                };
            },
            -0.1L
        ));
    });
}
}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    edge_case_tests();
    std::size_t case_count = 0;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- != 0){
        std::uint64_t seed = 0;
        std::cin >> seed;
        std::mt19937_64 random(seed);
        run_graph_and_allocation(random);
        run_knapsack();
        run_state_dp(random);
        std::cout << "0\n";
    }
}
