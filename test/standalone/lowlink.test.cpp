// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/connectivity/lowlink.hpp"

int components(int n, const std::vector<std::pair<int, int>>& edges, int banned_vertex, int banned_edge){
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        if(id == banned_edge) continue;
        auto [u, v] = edges[static_cast<std::size_t>(id)];
        if(u == banned_vertex || v == banned_vertex) continue;
        graph[static_cast<std::size_t>(u)].push_back(v);
        graph[static_cast<std::size_t>(v)].push_back(u);
    }
    std::vector<char> seen(static_cast<std::size_t>(n), 0);
    if(banned_vertex != -1) seen[static_cast<std::size_t>(banned_vertex)] = 1;
    int result = 0;
    for(int s = 0; s < n; s++){
        if(seen[static_cast<std::size_t>(s)]) continue;
        result++;
        std::vector<int> stack{s};
        seen[static_cast<std::size_t>(s)] = 1;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            for(int to: graph[static_cast<std::size_t>(v)]){
                if(!seen[static_cast<std::size_t>(to)]){
                    seen[static_cast<std::size_t>(to)] = 1;
                    stack.push_back(to);
                }
            }
        }
    }
    return result;
}

std::vector<int> brute_articulation(int n, const std::vector<std::pair<int, int>>& edges){
    std::vector<int> result;
    int base = components(n, edges, -1, -1);
    for(int v = 0; v < n; v++){
        if(components(n, edges, v, -1) > base) result.push_back(v);
    }
    return result;
}

std::vector<int> brute_bridges(int n, const std::vector<std::pair<int, int>>& edges){
    std::vector<int> result;
    int base = components(n, edges, -1, -1);
    for(int id = 0; id < static_cast<int>(edges.size()); id++){
        if(components(n, edges, -1, id) > base) result.push_back(id);
    }
    return result;
}

void self_test(){
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 0}, {1, 3}};
        auto res = lowlink(4, edges);
        assert((res.articulation == std::vector<int>{1}));
        assert((res.bridges == std::vector<int>{3}));
    }
    {
        std::vector<std::pair<int, int>> edges = {{0, 1}, {0, 1}};
        auto res = lowlink(2, edges);
        assert(res.bridges.empty());
    }
    std::mt19937 rng(20260805);
    for(int n = 1; n <= 9; n++){
        for(int step = 0; step < 100; step++){
            std::vector<std::pair<int, int>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 3 == 0) edges.push_back({u, v});
                    if(rng() % 13 == 0) edges.push_back({u, v});
                }
            }
            auto res = lowlink(n, edges);
            assert(res.articulation == brute_articulation(n, edges));
            assert(res.bridges == brute_bridges(n, edges));
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        std::cin >> u >> v;
        edges.push_back({u, v});
    }
    auto res = lowlink(n, edges);
    std::cout << res.articulation.size() << ' ' << res.bridges.size() << '\n';
}
