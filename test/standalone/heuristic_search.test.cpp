// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/search/heuristic_search.hpp"

using Edge = std::pair<int, long long>;
using Graph = std::vector<std::vector<Edge>>;

std::vector<long long> dijkstra(const Graph& graph, int source){
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
            if(current_distance + cost < distance[static_cast<std::size_t>(to)]){
                distance[static_cast<std::size_t>(to)] = current_distance + cost;
                queue.emplace(distance[static_cast<std::size_t>(to)], to);
            }
        }
    }
    return distance;
}

long long path_cost(const Graph& graph, const std::vector<int>& path){
    long long result = 0;
    for(std::size_t index = 1; index < path.size(); ++index){
        const auto& edges = graph[static_cast<std::size_t>(path[index - 1])];
        const auto iterator = std::find_if(edges.begin(), edges.end(), [&](const Edge& edge){
            return edge.first == path[index];
        });
        assert(iterator != edges.end());
        result += iterator->second;
    }
    return result;
}

void self_test(){
    std::mt19937 random_engine(20260713U);
    for(int n = 1; n <= 30; ++n){
        for(int repetition = 0; repetition < 40; ++repetition){
            Graph graph(static_cast<std::size_t>(n));
            Graph reverse_graph(static_cast<std::size_t>(n));
            for(int from = 0; from < n; ++from){
                for(int to = 0; to < n; ++to){
                    if(from == to || random_engine() % 5U != 0U) continue;
                    const long long cost = static_cast<long long>(random_engine() % 20U + 1U);
                    graph[static_cast<std::size_t>(from)].emplace_back(to, cost);
                    reverse_graph[static_cast<std::size_t>(to)].emplace_back(from, cost);
                }
            }
            const int source = static_cast<int>(random_engine() % static_cast<unsigned int>(n));
            const int target = static_cast<int>(random_engine() % static_cast<unsigned int>(n));
            const auto expected = dijkstra(graph, source)[static_cast<std::size_t>(target)];
            const auto neighbors = [&](int vertex){
                return graph[static_cast<std::size_t>(vertex)];
            };
            const auto backward = [&](int vertex) -> std::list<Edge>{
                const auto& edges = reverse_graph[static_cast<std::size_t>(vertex)];
                return std::list<Edge>(edges.begin(), edges.end());
            };
            const auto goal = [target](int vertex){ return vertex == target; };
            const auto zero = [](int){ return 0LL; };
            const auto a_star = a_star_search(source, goal, neighbors, zero);
            const auto weighted = weighted_a_star_search(source, goal, neighbors, zero, 3.0);
            const auto greedy = greedy_best_first_search(source, goal, neighbors, zero);
            const auto bidirectional = bidirectional_a_star_search(
                source, target, neighbors, backward, zero, zero
            );
            const bool reachable = expected != std::numeric_limits<long long>::max() / 4;
            assert(static_cast<bool>(a_star) == reachable);
            assert(static_cast<bool>(weighted) == reachable);
            assert(static_cast<bool>(greedy) == reachable);
            assert(static_cast<bool>(bidirectional) == reachable);
            if(reachable){
                assert(*a_star.cost == expected);
                assert(*weighted.cost == expected);
                assert(*bidirectional.cost == expected);
                assert(a_star.path.front() == source && a_star.path.back() == target);
                assert(bidirectional.path.front() == source && bidirectional.path.back() == target);
                assert(path_cost(graph, a_star.path) == *a_star.cost);
                assert(path_cost(graph, weighted.path) == *weighted.cost);
                assert(path_cost(graph, greedy.path) == *greedy.cost);
                assert(path_cost(graph, bidirectional.path) == *bidirectional.cost);
            }
        }
    }
    struct CustomState{
        int value;
        bool operator==(const CustomState&) const = default;
    };
    const auto custom_neighbors = [](const CustomState& state){
        return state.value < 4
            ? std::vector<std::pair<CustomState, int>>{{CustomState{state.value + 1}, 1}}
            : std::vector<std::pair<CustomState, int>>{};
    };
    const auto custom_hash = [](const CustomState& state){
        return std::hash<int>{}(state.value);
    };
    const auto custom_result = a_star_search(
        CustomState{0}, [](const CustomState& state){ return state.value == 4; },
        custom_neighbors, [](const CustomState& state){ return 4 - state.value; },
        custom_hash
    );
    assert(custom_result && *custom_result.cost == 4);

    const auto negative_neighbors = [](int vertex){
        return std::vector<Edge>{{vertex + 1, -1}};
    };
    bool threw = false;
    try{
        static_cast<void>(a_star_search(
            0, [](int vertex){ return vertex == 2; },
            negative_neighbors, [](int){ return 0LL; }
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

int main(){
    int n;
    int m;
    int source;
    int target;
    if(!(std::cin >> n >> m >> source >> target)){
        self_test();
        return 0;
    }
    Graph graph(static_cast<std::size_t>(n));
    Graph reverse_graph(static_cast<std::size_t>(n));
    for(int edge_index = 0; edge_index < m; ++edge_index){
        int from;
        int to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].emplace_back(to, cost);
        reverse_graph[static_cast<std::size_t>(to)].emplace_back(from, cost);
    }
    const auto neighbors = [&](int vertex){ return graph[static_cast<std::size_t>(vertex)]; };
    const auto backward = [&](int vertex){ return reverse_graph[static_cast<std::size_t>(vertex)]; };
    const auto goal = [target](int vertex){ return vertex == target; };
    const auto zero = [](int){ return 0LL; };
    const auto a_star = a_star_search(source, goal, neighbors, zero);
    const auto weighted = weighted_a_star_search(source, goal, neighbors, zero, 2.5);
    const auto greedy = greedy_best_first_search(source, goal, neighbors, zero);
    const auto bidirectional = bidirectional_a_star_search(
        source, target, neighbors, backward, zero, zero
    );
    const auto value = [](const auto& result){ return result.cost.value_or(-1); };
    std::cout << value(a_star) << ' ' << value(weighted) << ' '
              << (greedy ? 1 : 0) << ' ' << value(bidirectional) << '\n';
}
