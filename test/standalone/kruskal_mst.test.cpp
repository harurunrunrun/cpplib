// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/spanning_tree/kruskal_mst.hpp"

long long brute(
    int n,
    const std::vector<KruskalEdge<long long>>& edges,
    bool& connected
){
    const int m = static_cast<int>(edges.size());
    long long best = (1LL << 60);
    connected = false;
    for(int mask = 0; mask < (1 << m); mask++){
        if(__builtin_popcount(static_cast<unsigned>(mask)) != std::max(0, n - 1)) continue;
        kruskal_mst_internal::Dsu dsu(n);
        long long cost = 0;
        int count = 0;
        for(int i = 0; i < m; i++){
            if(mask >> i & 1){
                const auto& e = edges[static_cast<std::size_t>(i)];
                if(dsu.merge(e.from, e.to)){
                    cost += e.cost;
                    count++;
                }
            }
        }
        if(count == std::max(0, n - 1)){
            connected = true;
            best = std::min(best, cost);
        }
    }
    if(!connected) return 0;
    return best;
}

void self_test(){
    {
        std::vector<KruskalEdge<long long>> edges = {
            {0, 1, 1},
            {1, 2, 2},
            {0, 2, 10},
            {2, 3, 3},
        };
        auto res = kruskal_mst<long long>(4, edges);
        assert(res.connected);
        assert(res.cost == 6);
        assert(res.edge_ids.size() == 3);
    }
    std::mt19937 rng(20260815);
    for(int n = 0; n <= 8; n++){
        for(int step = 0; step < 100; step++){
            std::vector<KruskalEdge<long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = u + 1; v < n; v++){
                    if(rng() % 3 == 0 && static_cast<int>(edges.size()) < 20){
                        edges.push_back({u, v, static_cast<int>(rng() % 40) - 15});
                    }
                }
            }
            bool connected;
            auto expected = brute(n, edges, connected);
            auto actual = kruskal_mst<long long>(n, edges);
            assert(actual.connected == connected);
            if(connected) assert(actual.cost == expected);
        }
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<KruskalEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }
    auto res = kruskal_mst<long long>(n, edges);
    std::cout << (res.connected ? res.cost : -1) << '\n';
}
