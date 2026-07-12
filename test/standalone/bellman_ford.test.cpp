// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/bellman_ford.hpp"

long long brute_force_dist(
    int n,
    const std::vector<BellmanFordEdge<long long>>& edges,
    int s,
    int t
){
    constexpr long long INF = (1LL << 60);
    std::vector<long long> dp(static_cast<std::size_t>(n), INF);
    dp[static_cast<std::size_t>(s)] = 0;
    for(int iter = 0; iter < n - 1; iter++){
        auto next = dp;
        for(auto e: edges){
            if(dp[static_cast<std::size_t>(e.from)] == INF) continue;
            next[static_cast<std::size_t>(e.to)] = std::min(
                next[static_cast<std::size_t>(e.to)],
                dp[static_cast<std::size_t>(e.from)] + e.cost
            );
        }
        dp.swap(next);
    }
    return dp[static_cast<std::size_t>(t)];
}

int main(){
    {
        std::vector<BellmanFordEdge<long long>> edges = {
            {0, 1, 2},
            {1, 2, -5},
            {0, 2, 10},
        };
        auto res = bellman_ford<long long>(3, edges, 0);
        assert(res.dist[2] == -3);
        assert(!res.has_negative_cycle);
        assert(!res.negative[2]);
    }
    {
        std::vector<BellmanFordEdge<long long>> edges = {
            {0, 1, 1},
            {1, 2, -3},
            {2, 1, 1},
            {2, 3, 1},
        };
        auto res = bellman_ford<long long>(4, edges, 0);
        assert(res.has_negative_cycle);
        assert(res.negative[1]);
        assert(res.negative[2]);
        assert(res.negative[3]);
    }

    std::mt19937 rng(20260726);
    for(int n = 1; n <= 8; n++){
        for(int step = 0; step < 200; step++){
            std::vector<BellmanFordEdge<long long>> edges;
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(rng() % 3 == 0){
                        edges.push_back({u, v, static_cast<int>(rng() % 11)});
                    }
                }
            }
            int s = static_cast<int>(rng() % n);
            auto res = bellman_ford<long long>(n, edges, s, 1LL << 60);
            assert(!res.has_negative_cycle);
            for(int v = 0; v < n; v++){
                assert(res.dist[static_cast<std::size_t>(v)] == brute_force_dist(n, edges, s, v));
            }
        }
    }
}
