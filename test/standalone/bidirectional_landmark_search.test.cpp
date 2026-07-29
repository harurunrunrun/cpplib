// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/approximate/search/alt_landmark_search.hpp"
#include "../../src/approximate/search/front_to_front_bidirectional_search.hpp"
#include "../../src/approximate/search/mm_search.hpp"
#include "../../src/approximate/search/near_optimal_bidirectional_search.hpp"

using Edge = std::pair<int, long long>;
using Graph = std::vector<std::vector<Edge>>;

std::vector<long long> shortest_paths(const Graph& graph, int source){
    const long long infinity = std::numeric_limits<long long>::max() / 4;
    std::vector<long long> distance(graph.size(), infinity);
    using Entry = std::pair<long long, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.emplace(0, source);
    while(!queue.empty()){
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(current_distance != distance[static_cast<std::size_t>(vertex)]) continue;
        for(const auto [to, cost] : graph[static_cast<std::size_t>(vertex)]){
            const long long candidate = current_distance + cost;
            if(candidate < distance[static_cast<std::size_t>(to)]){
                distance[static_cast<std::size_t>(to)] = candidate;
                queue.emplace(candidate, to);
            }
        }
    }
    return distance;
}

void verify_path(
    const Graph& graph,
    int source,
    int target,
    const HeuristicSearchResult<int, long long>& result
){
    assert(result);
    assert(!result.path.empty());
    assert(result.path.front() == source);
    assert(result.path.back() == target);
    long long cost = 0;
    for(std::size_t i = 1; i < result.path.size(); ++i){
        const auto& adjacent = graph[static_cast<std::size_t>(result.path[i - 1])];
        const auto iterator = std::find_if(
            adjacent.begin(), adjacent.end(),
            [&](const Edge& edge){ return edge.first == result.path[i]; }
        );
        assert(iterator != adjacent.end());
        cost += iterator->second;
    }
    assert(cost == *result.cost);
}

void fixed_self_test(){
    Graph graph{
        {{1, 7}, {2, 2}},
        {{3, 3}, {4, 20}},
        {{1, 1}, {3, 10}},
        {{4, 2}},
        {}
    };
    Graph reverse_graph(graph.size());
    for(std::size_t from = 0; from < graph.size(); ++from){
        for(const auto [to, cost] : graph[from]){
            reverse_graph[static_cast<std::size_t>(to)].emplace_back(
                static_cast<int>(from), cost
            );
        }
    }
    std::vector<std::vector<long long>> distance;
    for(int source = 0; source < static_cast<int>(graph.size()); ++source){
        distance.push_back(shortest_paths(graph, source));
    }
    const auto forward = [&](int vertex){
        return graph[static_cast<std::size_t>(vertex)];
    };
    const auto backward = [&](int vertex){
        return reverse_graph[static_cast<std::size_t>(vertex)];
    };
    const auto pair_heuristic = [&](int from, int to){
        const long long value = distance[static_cast<std::size_t>(from)]
                                       [static_cast<std::size_t>(to)];
        return value == std::numeric_limits<long long>::max() / 4 ? 0LL : value;
    };
    const auto forward_heuristic = [&](int vertex){
        return pair_heuristic(vertex, 4);
    };
    const auto backward_heuristic = [&](int vertex){
        return pair_heuristic(0, vertex);
    };

    const auto front_to_front = front_to_front_bidirectional_search(
        0, 4, forward, backward, pair_heuristic
    );
    const auto mm = mm_search(
        0, 4, forward, backward, forward_heuristic, backward_heuristic
    );
    const auto nbs = nbs_search(
        0, 4, forward, backward, forward_heuristic, backward_heuristic
    );
    const auto landmarks = make_alt_landmark_heuristic(
        std::vector<int>{0, 3}, forward, backward
    );
    const auto alt = alt_a_star_search(0, 4, forward, landmarks);
    for(const auto& result : std::vector{
            front_to_front, mm, nbs, alt
        }){
        assert(result && *result.cost == 8);
        verify_path(graph, 0, 4, result);
    }
    assert(landmarks.landmark_count() == 2);
    assert(landmarks(0, 4) == 8);

    struct State{
        int value;
        bool operator==(const State&) const = default;
    };
    const auto hash = [](const State& state){
        return std::hash<int>{}(state.value);
    };
    const auto next = [](const State& state){
        return state.value < 3
            ? std::vector<std::pair<State, int>>{{State{state.value + 1}, 1}}
            : std::vector<std::pair<State, int>>{};
    };
    const auto previous = [](const State& state){
        return state.value > 0
            ? std::vector<std::pair<State, int>>{{State{state.value - 1}, 1}}
            : std::vector<std::pair<State, int>>{};
    };
    const auto custom = mm_search(
        State{0}, State{3}, next, previous,
        [](const State& state){ return 3 - state.value; },
        [](const State& state){ return state.value; },
        hash
    );
    assert(custom && *custom.cost == 3);
}

int main(){
    fixed_self_test();

    int n;
    int m;
    int source;
    int target;
    if(!(std::cin >> n >> m >> source >> target)) return 0;
    Graph graph(static_cast<std::size_t>(n));
    Graph reverse_graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int from;
        int to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].emplace_back(to, cost);
        reverse_graph[static_cast<std::size_t>(to)].emplace_back(from, cost);
    }
    const auto forward = [&](int vertex){
        return graph[static_cast<std::size_t>(vertex)];
    };
    const auto backward = [&](int vertex){
        return reverse_graph[static_cast<std::size_t>(vertex)];
    };
    const auto zero = [](int){ return 0LL; };
    const auto pair_zero = [](int, int){ return 0LL; };
    const auto front_to_front = front_to_front_bidirectional_search(
        source, target, forward, backward, pair_zero
    );
    const auto mm = mm_search(
        source, target, forward, backward, zero, zero
    );
    const auto nbs = nbs_search(
        source, target, forward, backward, zero, zero
    );
    std::vector<int> landmark_vertices;
    landmark_vertices.push_back(source);
    if(target != source) landmark_vertices.push_back(target);
    const auto landmarks = make_alt_landmark_heuristic(
        landmark_vertices, forward, backward
    );
    const auto alt = alt_a_star_search(
        source, target, forward, landmarks
    );

    const long long infinity = std::numeric_limits<long long>::max() / 4;
    const long long expected_distance = shortest_paths(
        graph, source
    )[static_cast<std::size_t>(target)];
    const bool reachable = expected_distance != infinity;
    for(const auto& result : std::vector{
            front_to_front, mm, nbs, alt
        }){
        assert(static_cast<bool>(result) == reachable);
        if(reachable){
            assert(*result.cost == expected_distance);
            verify_path(graph, source, target, result);
        }
    }
    const auto value = [](const auto& result){
        return result.cost.value_or(-1);
    };
    std::cout << value(front_to_front) << ' '
              << value(mm) << ' '
              << value(nbs) << ' '
              << value(alt) << '\n';
}
