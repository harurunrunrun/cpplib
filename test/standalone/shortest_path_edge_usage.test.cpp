// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/shortest_path_edge_usage.hpp"

std::vector<char> brute_used(
    int n,
    const std::vector<ShortestPathEdgeUsageEdge<long long>>& edges,
    int s,
    int t
){
    long long best = (1LL << 60);
    std::vector<char> used_edge(edges.size(), 0), seen(static_cast<std::size_t>(n), 0);
    std::vector<int> path_edges;
    auto dfs = [&](auto&& self, int v, long long cost) -> void {
        if(cost > best) return;
        if(v == t){
            if(cost < best){
                best = cost;
                std::fill(used_edge.begin(), used_edge.end(), 0);
            }
            if(cost == best){
                for(int id: path_edges) used_edge[static_cast<std::size_t>(id)] = 1;
            }
            return;
        }
        seen[static_cast<std::size_t>(v)] = 1;
        for(std::size_t i = 0; i < edges.size(); i++){
            const auto& e = edges[i];
            if(e.from != v || seen[static_cast<std::size_t>(e.to)]) continue;
            path_edges.push_back(static_cast<int>(i));
            self(self, e.to, cost + e.cost);
            path_edges.pop_back();
        }
        seen[static_cast<std::size_t>(v)] = 0;
    };
    dfs(dfs, s, 0);
    return used_edge;
}

void self_test(){
    {
        std::vector<ShortestPathEdgeUsageEdge<long long>> edges = {
            {0, 1, 1},
            {1, 3, 2},
            {0, 2, 1},
            {2, 3, 2},
            {0, 3, 4},
        };
        auto res = shortest_path_edge_usage<long long>(4, edges, 0, 3);
        assert(res.shortest == 3);
        assert((res.used == std::vector<char>{1, 1, 1, 1, 0}));
        assert(res.unused[4]);
    }
    std::mt19937 rng(20260802);
    for(int n = 1; n <= 8; n++){
        for(int step = 0; step < 200; step++){
            std::vector<ShortestPathEdgeUsageEdge<long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(u != v && rng() % 5 == 0){
                        edges.push_back({u, v, 1 + static_cast<int>(rng() % 8)});
                    }
                }
            }
            int s = static_cast<int>(rng() % n);
            int t = static_cast<int>(rng() % n);
            auto res = shortest_path_edge_usage<long long>(n, edges, s, t, 1LL << 60);
            assert(res.used == brute_used(n, edges, s, t));
        }
    }
}

int main(){
    int n, m, s, t;
    if(!(std::cin >> n >> m >> s >> t)){
        self_test();
        return 0;
    }
    std::vector<ShortestPathEdgeUsageEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }
    auto res = shortest_path_edge_usage<long long>(n, edges, s, t, 1LL << 60);
    for(int i = 0; i < m; i++){
        if(i) std::cout << ' ';
        std::cout << static_cast<int>(res.used[static_cast<std::size_t>(i)]);
    }
    std::cout << '\n';
}
