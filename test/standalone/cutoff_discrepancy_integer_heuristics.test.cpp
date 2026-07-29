// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/optimization/integer_heuristics.hpp"
#include "../../src/approximate/search/cutoff_search.hpp"
#include "../../src/approximate/search/discrepancy_search.hpp"
#include "../../src/approximate/search/heuristic_search.hpp"

namespace cutoff_discrepancy_integer_heuristics_test{

using Edge = std::pair<int, int>;
using Graph = std::vector<std::vector<Edge>>;

int dijkstra(const Graph& graph, int source, int target){
    constexpr int infinity = std::numeric_limits<int>::max() / 4;
    std::vector<int> distance(graph.size(), infinity);
    using Entry = std::pair<int, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.emplace(0, source);
    while(!queue.empty()){
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(current_distance != distance[static_cast<std::size_t>(vertex)]) continue;
        for(const auto [to, cost] : graph[static_cast<std::size_t>(vertex)]){
            const int candidate = current_distance + cost;
            if(candidate >= distance[static_cast<std::size_t>(to)]) continue;
            distance[static_cast<std::size_t>(to)] = candidate;
            queue.emplace(candidate, to);
        }
    }
    return distance[static_cast<std::size_t>(target)];
}

struct BinaryProblem{
    std::vector<int> weight;
    int target = 0;

    bool feasible(const std::vector<int>& values) const{
        int sum = 0;
        for(std::size_t i = 0; i < values.size(); ++i){
            sum += values[i] * weight[i];
        }
        return sum >= target;
    }

    int objective(const std::vector<int>& values) const{
        int sum = 0;
        for(std::size_t i = 0; i < values.size(); ++i){
            sum += values[i] * weight[i];
        }
        return sum;
    }

    int exact() const{
        int best = std::numeric_limits<int>::max() / 4;
        for(std::size_t mask = 0;
            mask < (std::size_t{1} << weight.size());
            ++mask){
            int sum = 0;
            for(std::size_t i = 0; i < weight.size(); ++i){
                if((mask >> i) & 1U) sum += weight[i];
            }
            if(sum >= target) best = std::min(best, sum);
        }
        return best;
    }

    std::optional<std::vector<int>> solve(
        const IntegerNeighborhoodRequest<int, int>& request
    ) const{
        std::optional<std::vector<int>> answer;
        int best_value = std::numeric_limits<int>::max() / 4;
        std::size_t best_distance = std::numeric_limits<std::size_t>::max();
        for(std::size_t mask = 0;
            mask < (std::size_t{1} << weight.size());
            ++mask){
            std::vector<int> values(weight.size());
            bool allowed = true;
            std::size_t distance = 0;
            for(std::size_t i = 0; i < weight.size(); ++i){
                values[i] = static_cast<int>((mask >> i) & 1U);
                if(i < request.fixed.size() && request.fixed[i]
                   && values[i] != *request.fixed[i]){
                    allowed = false;
                }
                if(i < request.reference.size()
                   && values[i] != request.reference[i]){
                    ++distance;
                }
            }
            if(!allowed || distance > request.maximum_distance
               || !feasible(values)){
                continue;
            }
            const int value = objective(values);
            if(request.objective_upper_bound
               && !(value < *request.objective_upper_bound)){
                continue;
            }
            const bool proximity =
                request.purpose == IntegerNeighborhoodPurpose::proximity;
            if(!answer
               || (proximity
                       ? std::pair(distance, value)
                             < std::pair(best_distance, best_value)
                       : std::pair(value, distance)
                             < std::pair(best_value, best_distance))){
                answer = values;
                best_value = value;
                best_distance = distance;
            }
        }
        return answer;
    }
};

struct SubsetNode{
    std::size_t index = 0;
    int sum = 0;
    std::vector<int> values;
};

int branch_and_bound_exact(const BinaryProblem& problem){
    const SubsetNode root{0, 0, {}};
    const auto complete = [&](const SubsetNode& node){
        return node.index == problem.weight.size();
    };
    const auto branch = [&](const SubsetNode& node){
        std::vector<SubsetNode> children;
        for(const int bit : {0, 1}){
            SubsetNode child = node;
            child.sum += bit * problem.weight[node.index];
            child.values.push_back(bit);
            ++child.index;
            children.push_back(std::move(child));
        }
        return children;
    };
    const auto objective = [&](const SubsetNode& node){
        return node.sum >= problem.target
            ? node.sum : std::numeric_limits<int>::max() / 4;
    };
    const auto lower_bound = [](const SubsetNode& node){ return node.sum; };
    const auto result = best_first_branch_and_bound(
        root, complete, branch, objective, lower_bound
    );
    assert(result && result.optimal);
    const auto interrupted = interrupted_branch_and_bound(
        root, complete, branch, objective, lower_bound, 0
    );
    assert(!interrupted.optimal);
    assert(interrupted.stop == CutoffSearchStop::expansion_limit);
    return *result.objective;
}

void exercise_graph_search(const Graph& graph, int target){
    const auto neighbors = [&](int vertex){
        return graph[static_cast<std::size_t>(vertex)];
    };
    const auto goal = [target](int vertex){ return vertex == target; };
    const auto zero = [](int){ return 0; };
    const int exact = dijkstra(graph, 0, target);
    const std::size_t n = graph.size();

    const auto bounded = bounded_suboptimal_search(
        0, goal, neighbors, zero, 1.0L
    );
    const auto depth = depth_bounded_a_star_search(
        0, goal, neighbors, zero, n
    );
    const auto cutoff = cutoff_a_star_search(
        0, goal, neighbors, zero, exact
    );
    const auto expansions = expansion_limited_a_star_search(
        0, goal, neighbors, zero, n * n + 1
    );
    const auto memory = memory_limited_a_star_search(
        0, goal, neighbors, zero, n * n + 1
    );
    const auto epsilon = epsilon_greedy_search(
        0, goal, neighbors, zero, 0.0L, n * n + 1, 123
    );
    assert(bounded.cost == exact);
    assert(depth.cost == exact);
    assert(cutoff.cost == exact);
    assert(expansions.cost == exact);
    assert(memory.cost == exact);
    assert(epsilon.cost == exact);

    const auto no_expansion = expansion_limited_a_star_search(
        0, goal, neighbors, zero, 0
    );
    assert(!no_expansion);
    assert(no_expansion.stop == CutoffSearchStop::expansion_limit);
    const auto too_shallow = depth_bounded_a_star_search(
        0, goal, neighbors, zero, 0
    );
    if(target != 0){
        assert(!too_shallow);
        assert(too_shallow.stop == CutoffSearchStop::depth_limit);
    }
    bool threw = false;
    try{
        static_cast<void>(memory_limited_a_star_search(
            0, goal, neighbors, zero, 0
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void exercise_discrepancy_search(){
    constexpr std::size_t depth = 5;
    constexpr std::size_t target = 61;
    const auto goal = [](std::size_t state){ return state == target; };
    const auto children = [](std::size_t state){
        if(state >= 32) return std::vector<std::size_t>{};
        return std::vector<std::size_t>{state * 2, state * 2 + 1};
    };
    const auto lds = limited_discrepancy_search(
        std::size_t{1}, goal, children, depth, depth
    );
    const auto dds = depth_bounded_discrepancy_search(
        std::size_t{1}, goal, children, depth
    );
    const auto ilds = improved_limited_discrepancy_search(
        std::size_t{1}, goal, children, depth, depth
    );
    const auto broadening = iterative_broadening_search(
        std::size_t{1}, goal, children, depth, 2
    );
    assert(lds && dds && ilds && broadening);
    assert(*lds.solution == target);
    assert(*dds.solution == target);
    assert(*ilds.solution == target);
    assert(*broadening.solution == target);

    const auto stopped = limited_discrepancy_search(
        std::size_t{1}, goal, children, depth, depth, 0
    );
    assert(!stopped);
    assert(stopped.stop == DiscrepancySearchStop::expansion_limit);
    bool threw = false;
    try{
        static_cast<void>(iterative_broadening_search(
            std::size_t{1}, goal, children, depth, 0
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void exercise_integer_heuristics(const BinaryProblem& problem){
    const std::size_t n = problem.weight.size();
    const std::vector<int> lower(n, 0);
    const std::vector<int> upper(n, 1);
    const std::vector<int> incumbent(n, 1);
    const int exact = problem.exact();
    const auto feasible = [&](const std::vector<int>& x){
        return problem.feasible(x);
    };
    const auto objective = [&](const std::vector<int>& x){
        return problem.objective(x);
    };
    const auto solve = [&](const IntegerNeighborhoodRequest<int, int>& request){
        return problem.solve(request);
    };

    const auto local = local_branching(
        incumbent, n, solve, feasible, objective, 3
    );
    assert(local && *local.objective == exact);

    std::vector<long double> relaxation(n, 0.0L);
    const auto exact_solution = problem.solve(
        IntegerNeighborhoodRequest<int, int>{
            std::vector<std::optional<int>>(n), {}, n, std::nullopt,
            IntegerNeighborhoodPurpose::rins
        }
    );
    assert(exact_solution);
    for(std::size_t i = 0; i < n; ++i){
        relaxation[i] = static_cast<long double>((*exact_solution)[i]);
    }
    const auto rins_result = rins(
        incumbent, relaxation, solve, feasible, objective
    );
    assert(rins_result);

    const auto pump = feasibility_pump(
        std::vector<long double>(n, 0.1L), lower, upper,
        feasible, objective,
        [&](const std::vector<int>&)
            -> std::optional<std::vector<long double>> {
            return relaxation;
        },
        3
    );
    assert(pump && *pump.objective == exact);

    std::vector<std::vector<std::size_t>> blocks;
    for(std::size_t i = 0; i < n; ++i) blocks.push_back({i});
    const auto relax_fix = relax_and_fix<int>(
        n, blocks, solve, feasible, objective
    );
    assert(relax_fix);
    const auto fix_opt = fix_and_optimize(
        incumbent, blocks, solve, feasible, objective, 2
    );
    assert(fix_opt);
    assert(*fix_opt.objective <= problem.objective(incumbent));

    const auto dive = diving_heuristic(
        lower, upper,
        [&](const std::vector<std::optional<int>>& fixed)
            -> std::optional<std::vector<long double>> {
            std::vector<long double> values = relaxation;
            for(std::size_t i = 0; i < n; ++i){
                if(fixed[i]) values[i] = static_cast<long double>(*fixed[i]);
            }
            return values;
        },
        feasible, objective, n
    );
    assert(dive);

    const auto rounded = rounding_heuristic(
        std::vector<long double>(n, 0.1L), lower, upper,
        feasible, objective,
        [&](const std::vector<int>&)
            -> std::optional<std::vector<int>> {
            return exact_solution;
        }
    );
    assert(rounded && *rounded.objective == exact);

    const auto proximity = proximity_search(
        incumbent, n, solve, feasible, objective, 3
    );
    assert(proximity);
    assert(*proximity.objective <= problem.objective(incumbent));
    const auto polished = solution_polishing(
        incumbent, lower, upper, feasible, objective, n
    );
    assert(polished);
    assert(*polished.objective <= problem.objective(incumbent));
}

void randomized_self_test(std::uint64_t seed, int rounds){
    std::mt19937_64 random(seed);
    for(int round = 0; round < rounds; ++round){
        const int n = 2 + static_cast<int>(random() % 8);
        Graph graph(static_cast<std::size_t>(n));
        for(int vertex = 0; vertex + 1 < n; ++vertex){
            graph[static_cast<std::size_t>(vertex)].emplace_back(
                vertex + 1, 1 + static_cast<int>(random() % 9)
            );
        }
        for(int from = 0; from < n; ++from){
            for(int to = from + 2; to < n; ++to){
                if(random() % 4 == 0){
                    graph[static_cast<std::size_t>(from)].emplace_back(
                        to, 1 + static_cast<int>(random() % 9)
                    );
                }
            }
        }
        exercise_graph_search(graph, n - 1);
    }
    exercise_discrepancy_search();
}

}  // namespace cutoff_discrepancy_integer_heuristics_test

int main(){
    using namespace cutoff_discrepancy_integer_heuristics_test;
    int case_count;
    if(!(std::cin >> case_count)){
        randomized_self_test(0xC070FFULL, 50);
        exercise_integer_heuristics(BinaryProblem{{2, 3, 5, 7, 11}, 13});
        return 0;
    }
    for(int case_index = 0; case_index < case_count; ++case_index){
        int n;
        int target;
        std::cin >> n >> target;
        BinaryProblem problem;
        problem.target = target;
        problem.weight.resize(static_cast<std::size_t>(n));
        for(int& weight : problem.weight) std::cin >> weight;
        const int answer = branch_and_bound_exact(problem);
        assert(answer == problem.exact());
        exercise_integer_heuristics(problem);
        std::cout << answer << '\n';
    }
}
