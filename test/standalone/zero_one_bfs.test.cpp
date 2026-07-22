// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/zero_one_bfs.hpp"

std::vector<int> dijkstra_brute(
    const std::vector<std::vector<ZeroOneBFSEdge>>& graph,
    const std::vector<int>& sources
){
    const int inf = 1 << 28;
    std::vector<int> dist(graph.size(), inf);
    using Pair = std::pair<int, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> queue;
    for(int source: sources){
        if(dist[static_cast<std::size_t>(source)] == 0) continue;
        dist[static_cast<std::size_t>(source)] = 0;
        queue.push({0, source});
    }
    while(!queue.empty()){
        auto [distance, v] = queue.top();
        queue.pop();
        if(dist[static_cast<std::size_t>(v)] != distance) continue;
        for(const auto& edge: graph[static_cast<std::size_t>(v)]){
            if(distance + edge.cost >= dist[static_cast<std::size_t>(edge.to)]) continue;
            dist[static_cast<std::size_t>(edge.to)] = distance + edge.cost;
            queue.push({dist[static_cast<std::size_t>(edge.to)], edge.to});
        }
    }
    for(int& distance: dist) if(distance == inf) distance = -1;
    return dist;
}

void validate_paths(
    const std::vector<std::vector<ZeroOneBFSEdge>>& graph,
    const ZeroOneBFSResult& result
){
    for(int target = 0; target < static_cast<int>(graph.size()); ++target){
        auto path = result.path(target);
        if(!result.reachable[static_cast<std::size_t>(target)]){
            assert(path.empty());
            continue;
        }
        assert(path.front() == result.source[static_cast<std::size_t>(target)]);
        assert(path.back() == target);
        for(std::size_t i = 1; i < path.size(); ++i){
            const int from = path[i - 1];
            const int to = path[i];
            assert(result.parent[static_cast<std::size_t>(to)] == from);
            const int difference = result.dist[static_cast<std::size_t>(to)] -
                result.dist[static_cast<std::size_t>(from)];
            assert(std::any_of(
                graph[static_cast<std::size_t>(from)].begin(),
                graph[static_cast<std::size_t>(from)].end(),
                [&](const ZeroOneBFSEdge& edge){
                    return edge.to == to && edge.cost == difference;
                }
            ));
        }
    }
}

void self_test(){
    {
        std::vector<std::vector<ZeroOneBFSEdge>> graph(6);
        graph[0] = {{1, 1}, {2, 0}};
        graph[2] = {{1, 0}, {4, 1}};
        graph[3] = {{4, 0}};
        graph[4] = {{5, 1}};
        auto result = zero_one_bfs(graph, std::vector<int>{0, 3, 3});
        assert((result.dist == std::vector<int>{0, 0, 0, 0, 0, 1}));
        validate_paths(graph, result);
    }

    std::mt19937 rng(20260831);
    for(int n = 1; n <= 35; ++n){
        for(int step = 0; step < 80; ++step){
            std::vector<std::vector<ZeroOneBFSEdge>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; ++u){
                for(int v = 0; v < n; ++v){
                    if(rng() % 7 == 0){
                        graph[static_cast<std::size_t>(u)].push_back({v, static_cast<int>(rng() & 1)});
                    }
                }
            }
            std::vector<int> sources;
            for(int v = 0; v < n; ++v) if(rng() % 5 == 0) sources.push_back(v);
            auto result = zero_one_bfs(graph, sources);
            assert(result.dist == dijkstra_brute(graph, sources));
            validate_paths(graph, result);

            auto implicit_result = zero_one_bfs_implicit(
                n,
                sources,
                [&](int vertex, const auto& relax){
                    for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
                        relax(edge.to, edge.cost);
                    }
                }
            );
            assert(implicit_result.dist == result.dist);
            assert(implicit_result.source == result.source);
            validate_paths(graph, implicit_result);
        }
    }

    bool thrown = false;
    try{
        (void)zero_one_bfs(std::vector<std::vector<ZeroOneBFSEdge>>{{{0, 2}}}, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)zero_one_bfs_implicit(
            1,
            0,
            [](int, const auto& relax){ relax(0, 2); }
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)zero_one_bfs_implicit(1, -1, [](int, const auto&){});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)zero_one_bfs_implicit(
            -1,
            std::vector<int>{},
            [](int, const auto&){}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n, m, k;
    if(!(std::cin >> n >> m >> k)){
        self_test();
        return 0;
    }
    std::vector<int> sources(static_cast<std::size_t>(k));
    for(int& source: sources) std::cin >> source;
    std::vector<std::vector<ZeroOneBFSEdge>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int from, to, cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
    }
    auto result = zero_one_bfs(graph, sources);
    for(int v = 0; v < n; ++v){
        if(v != 0) std::cout << ' ';
        std::cout << result.dist[static_cast<std::size_t>(v)];
    }
    std::cout << '\n';
}
