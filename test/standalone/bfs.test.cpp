// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/traversal/bfs.hpp"

std::vector<int> brute_dist(
    const std::vector<std::vector<int>>& graph,
    const std::vector<int>& sources
){
    std::vector<int> dist(graph.size(), -1);
    std::queue<int> queue;
    for(int source: sources){
        if(dist[static_cast<std::size_t>(source)] != -1) continue;
        dist[static_cast<std::size_t>(source)] = 0;
        queue.push(source);
    }
    while(!queue.empty()){
        int v = queue.front();
        queue.pop();
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(dist[static_cast<std::size_t>(to)] != -1) continue;
            dist[static_cast<std::size_t>(to)] = dist[static_cast<std::size_t>(v)] + 1;
            queue.push(to);
        }
    }
    return dist;
}

void validate_paths(const std::vector<std::vector<int>>& graph, const BFSResult& result){
    for(int target = 0; target < static_cast<int>(graph.size()); ++target){
        auto path = result.path(target);
        if(!result.reachable[static_cast<std::size_t>(target)]){
            assert(path.empty());
            assert(result.dist[static_cast<std::size_t>(target)] == -1);
            continue;
        }
        assert(!path.empty());
        assert(path.front() == result.source[static_cast<std::size_t>(target)]);
        assert(path.back() == target);
        assert(static_cast<int>(path.size()) == result.dist[static_cast<std::size_t>(target)] + 1);
        for(std::size_t i = 1; i < path.size(); ++i){
            assert(result.parent[static_cast<std::size_t>(path[i])] == path[i - 1]);
            const auto& edges = graph[static_cast<std::size_t>(path[i - 1])];
            assert(std::find(edges.begin(), edges.end(), path[i]) != edges.end());
        }
    }
}

void self_test(){
    {
        std::vector<std::vector<int>> graph(6);
        graph[0] = {1, 2};
        graph[3] = {2, 4};
        graph[2] = {5};
        auto result = bfs(graph, std::vector<int>{0, 3, 0});
        assert((result.dist == std::vector<int>{0, 1, 1, 0, 1, 2}));
        assert(result.source[2] == 0);
        validate_paths(graph, result);
    }
    {
        std::vector<std::vector<int>> graph(3);
        auto result = bfs(graph, std::vector<int>{});
        assert((result.dist == std::vector<int>{-1, -1, -1}));
        validate_paths(graph, result);
    }

    std::mt19937 rng(20260830);
    for(int n = 1; n <= 35; ++n){
        for(int step = 0; step < 60; ++step){
            std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; ++u){
                for(int v = 0; v < n; ++v){
                    if(rng() % 7 == 0) graph[static_cast<std::size_t>(u)].push_back(v);
                }
            }
            std::vector<int> sources;
            for(int v = 0; v < n; ++v){
                if(rng() % 5 == 0) sources.push_back(v);
            }
            auto result = bfs(graph, sources);
            assert(result.dist == brute_dist(graph, sources));
            validate_paths(graph, result);
        }
    }

    bool thrown = false;
    try{
        (void)bfs(std::vector<std::vector<int>>{{1}}, 0);
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
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    auto result = bfs(graph, sources);
    for(int v = 0; v < n; ++v){
        if(v != 0) std::cout << ' ';
        std::cout << result.dist[static_cast<std::size_t>(v)];
    }
    std::cout << '\n';
}
