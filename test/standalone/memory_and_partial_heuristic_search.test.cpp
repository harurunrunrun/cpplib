// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/search/depth_first_heuristic_search.hpp"
#include "../../src/approximate/search/memory_bounded_heuristic_search.hpp"
#include "../../src/approximate/search/partial_expansion_search.hpp"

using Edge = std::pair<int, long long>;
using Graph = std::vector<std::vector<Edge>>;

std::vector<long long> distances_to(const Graph& graph, int target){
    const long long infinity = std::numeric_limits<long long>::max() / 4;
    Graph reverse(graph.size());
    for(std::size_t from = 0; from < graph.size(); ++from){
        for(const auto [to, cost] : graph[from]){
            reverse[static_cast<std::size_t>(to)].emplace_back(
                static_cast<int>(from), cost
            );
        }
    }
    std::vector<long long> distance(graph.size(), infinity);
    using Entry = std::pair<long long, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;
    distance[static_cast<std::size_t>(target)] = 0;
    queue.emplace(0, target);
    while(!queue.empty()){
        const auto [current_distance, vertex] = queue.top();
        queue.pop();
        if(current_distance != distance[static_cast<std::size_t>(vertex)]) continue;
        for(const auto [to, cost] : reverse[static_cast<std::size_t>(vertex)]){
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
    long long expected,
    const HeuristicSearchResult<int, long long>& result
){
    if(expected < 0){
        assert(!result);
        return;
    }
    assert(result);
    assert(*result.cost == expected);
    assert(!result.path.empty());
    assert(result.path.front() == source);
    assert(result.path.back() == target);
    long long actual = 0;
    for(std::size_t i = 1; i < result.path.size(); ++i){
        const auto& adjacent = graph[static_cast<std::size_t>(result.path[i - 1])];
        const auto iterator = std::find_if(
            adjacent.begin(), adjacent.end(),
            [&](const Edge& edge){ return edge.first == result.path[i]; }
        );
        assert(iterator != adjacent.end());
        actual += iterator->second;
    }
    assert(actual == expected);
}

int main(){
    int n;
    int m;
    int source;
    int target;
    long long expected;
    if(!(std::cin >> n >> m >> source >> target >> expected)) return 0;
    Graph graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int from;
        int to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].emplace_back(to, cost);
    }
    const std::vector<long long> distance = distances_to(graph, target);
    const long long infinity = std::numeric_limits<long long>::max() / 4;
    const auto neighbors = [&](int vertex) -> const std::vector<Edge>& {
        return graph[static_cast<std::size_t>(vertex)];
    };
    const auto goal = [target](int state){ return state == target; };
    const auto heuristic = [&](int state){
        const long long value = distance[static_cast<std::size_t>(state)];
        return value == infinity ? 0LL : value;
    };
    const std::size_t generous_limit =
        static_cast<std::size_t>(std::max(32, n * n + 1));

    const auto weighted_ida = weighted_ida_star_search(
        source, goal, neighbors, heuristic, 1.0
    );
    const auto rbfs = recursive_best_first_search(
        source, goal, neighbors, heuristic
    );
    const auto sma = simplified_memory_bounded_a_star_search(
        source, goal, neighbors, heuristic, generous_limit
    );
    const auto ma = memory_bounded_a_star_search(
        source, goal, neighbors, heuristic, generous_limit
    );
    const auto fringe = fringe_search(source, goal, neighbors, heuristic);
    const auto partial = partial_expansion_a_star_search(
        source, goal, neighbors, heuristic
    );
    const auto pea = pea_star_search(source, goal, neighbors, heuristic);

    verify_path(graph, source, target, expected, weighted_ida);
    verify_path(graph, source, target, expected, rbfs);
    verify_path(graph, source, target, expected, sma);
    verify_path(graph, source, target, expected, ma);
    verify_path(graph, source, target, expected, fringe);
    verify_path(graph, source, target, expected, partial);
    verify_path(graph, source, target, expected, pea);

    bool threw = false;
    try{
        static_cast<void>(simplified_memory_bounded_a_star_search(
            source, goal, neighbors, heuristic, 0
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
    threw = false;
    try{
        static_cast<void>(weighted_ida_star_search(
            source, goal, neighbors, heuristic, 0.5
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
    std::cout << expected << '\n';
}
