// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/bellman_ford.hpp"

struct BruteDistance{
    bool reachable;
    long long value;
};

BruteDistance brute_force_dist(
    int n,
    const std::vector<BellmanFordEdge<long long>>& edges,
    int s,
    int t
){
    std::vector<long long> dist(static_cast<std::size_t>(n));
    std::vector<char> reachable(static_cast<std::size_t>(n), 0);
    dist[static_cast<std::size_t>(s)] = 0;
    reachable[static_cast<std::size_t>(s)] = 1;
    for(int iter = 0; iter < n - 1; iter++){
        auto next_dist = dist;
        auto next_reachable = reachable;
        for(auto e: edges){
            if(!reachable[static_cast<std::size_t>(e.from)]) continue;
            long long candidate = dist[static_cast<std::size_t>(e.from)] + e.cost;
            if(!next_reachable[static_cast<std::size_t>(e.to)] ||
               candidate < next_dist[static_cast<std::size_t>(e.to)]){
                next_dist[static_cast<std::size_t>(e.to)] = candidate;
                next_reachable[static_cast<std::size_t>(e.to)] = 1;
            }
        }
        dist.swap(next_dist);
        reachable.swap(next_reachable);
    }
    return {static_cast<bool>(reachable[static_cast<std::size_t>(t)]),
            dist[static_cast<std::size_t>(t)]};
}

int main(){
    int test_count;
    if(std::cin >> test_count){
        constexpr long long INF = 1LL << 60;
        while(test_count--){
            int n, m, source;
            std::cin >> n >> m >> source;
            std::vector<BellmanFordEdge<long long>> edges(static_cast<std::size_t>(m));
            for(auto& edge: edges){
                std::cin >> edge.from >> edge.to >> edge.cost;
            }
            const auto result = bellman_ford<long long>(n, edges, source, INF);
            for(int v = 0; v < n; v++){
                if(v != 0) std::cout << ' ';
                if(result.negative[static_cast<std::size_t>(v)]){
                    std::cout << "NEG";
                }else if(!result.reachable[static_cast<std::size_t>(v)]){
                    std::cout << "INF";
                }else{
                    std::cout << result.dist[static_cast<std::size_t>(v)];
                }
            }
            std::cout << '\n';
        }
        return 0;
    }

    constexpr long long INF = 1LL << 60;
    {
        std::vector<BellmanFordEdge<long long>> edges = {
            {0, 1, 2},
            {1, 2, -5},
            {0, 2, 10},
        };
        auto res = bellman_ford<long long>(3, edges, 0);
        assert(res.reachable[2]);
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
    {
        std::vector<BellmanFordEdge<long long>> edges = {
            {0, 1, INF},
            {1, 2, 2000000000000000000LL},
            {3, 3, -1},
        };
        auto res = bellman_ford<long long>(4, edges, 0, INF);
        assert(res.reachable[1] && res.dist[1] == INF);
        assert(res.reachable[2] && res.dist[2] == INF + 2000000000000000000LL);
        assert(!res.reachable[3] && res.dist[3] == INF);
        assert(!res.has_negative_cycle);
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
            auto res = bellman_ford<long long>(n, edges, s, INF);
            assert(!res.has_negative_cycle);
            for(int v = 0; v < n; v++){
                auto expected = brute_force_dist(n, edges, s, v);
                assert(static_cast<bool>(res.reachable[static_cast<std::size_t>(v)]) == expected.reachable);
                if(expected.reachable){
                    assert(res.dist[static_cast<std::size_t>(v)] == expected.value);
                }
            }
        }
    }
}
