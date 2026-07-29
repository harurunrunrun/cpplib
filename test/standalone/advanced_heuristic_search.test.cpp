// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/search/advanced_heuristic_search.hpp"

using Edge = std::pair<int, long long>;
using Graph = std::vector<std::vector<Edge>>;

long long shortest_path(const Graph& graph, int source, int target){
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
    return distance[static_cast<std::size_t>(target)] == infinity
        ? -1 : distance[static_cast<std::size_t>(target)];
}

void verify_path(
    const Graph& graph,
    int source,
    int target,
    const HeuristicSearchResult<int, long long>& result
){
    assert(result.cost);
    assert(!result.path.empty());
    assert(result.path.front() == source);
    assert(result.path.back() == target);
    long long cost = 0;
    for(std::size_t i = 1; i < result.path.size(); ++i){
        const auto& edges = graph[static_cast<std::size_t>(result.path[i - 1])];
        const auto iterator = std::find_if(
            edges.begin(), edges.end(),
            [&](const Edge& edge){ return edge.first == result.path[i]; }
        );
        assert(iterator != edges.end());
        cost += iterator->second;
    }
    assert(cost == *result.cost);
}

int main(){
    int n;
    int m;
    int source;
    int target;
    if(!(std::cin >> n >> m >> source >> target)) return 0;
    Graph graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int from;
        int to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].emplace_back(to, cost);
    }
    const auto neighbors = [&](int vertex){
        return graph[static_cast<std::size_t>(vertex)];
    };
    const auto goal = [target](int vertex){ return vertex == target; };
    const auto zero = [](int){ return 0LL; };
    const auto secondary = [](int vertex){ return vertex; };
    const auto multi = [](int){ return std::vector<long long>{0, 1}; };
    const long long expected = shortest_path(graph, source, target);

    const auto anytime = anytime_a_star_search(
        source, goal, neighbors, zero, 2.0
    );
    const auto ara = ara_star_search(
        source, goal, neighbors, zero, std::vector<double>{3.0, 2.0, 1.0}
    );
    const auto ana = ana_star_search(source, goal, neighbors, zero);
    const auto focal = focal_search(
        source, goal, neighbors, zero, secondary, 1.5
    );
    const auto ae = a_epsilon_search(source, goal, neighbors, zero, 0.5);
    const auto ees = explicit_estimation_search(
        source, goal, neighbors, zero, zero, zero, 1.5
    );
    const auto optimistic = optimistic_search(
        source, goal, neighbors, zero, 1.5
    );
    const auto potential = potential_search(
        source, goal, neighbors, zero,
        expected < 0 ? 1LL : expected + 1
    );
    const auto mh = multi_heuristic_a_star_search(
        source, goal, neighbors, multi, 1.0
    );
    const auto mha = mha_star_search(
        source, goal, neighbors, zero, multi, 1.0, 1.0
    );
    const auto dynamic = dynamic_weighted_a_star_search(
        source, goal, neighbors, zero, 2.0,
        static_cast<std::size_t>(std::max(n, 1))
    );

    if(expected < 0){
        assert(!anytime && !ara && !ana);
        assert(!focal && !ae && !ees && !optimistic && !potential);
        assert(!mh && !mha && !dynamic);
    }else{
        assert(anytime && ara && ana);
        assert(*anytime.best().cost == expected);
        assert(*ara.best().cost == expected);
        assert(*ana.best().cost == expected);
        const std::vector<HeuristicSearchResult<int, long long>> results{
            focal, ae, ees, optimistic, potential, mh, mha, dynamic
        };
        for(const auto& result : results){
            assert(*result.cost == expected);
            verify_path(graph, source, target, result);
        }
    }

    bool threw = false;
    try{
        static_cast<void>(a_epsilon_search(
            source, goal, neighbors, zero, -0.1
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
    std::cout << expected << '\n';
}
