// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/dijkstra.hpp"

long long brute_force_dist(
    const std::vector<std::vector<DijkstraEdge<long long>>>& graph,
    int s,
    int t
){
    constexpr long long INF = (1LL << 60);
    const int n = static_cast<int>(graph.size());
    std::vector<long long> dp(static_cast<std::size_t>(n), INF);
    dp[static_cast<std::size_t>(s)] = 0;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(int v = 0; v < n; v++){
            if(dp[static_cast<std::size_t>(v)] == INF) continue;
            for(auto e: graph[static_cast<std::size_t>(v)]){
                long long nd = dp[static_cast<std::size_t>(v)] + e.cost;
                if(nd < dp[static_cast<std::size_t>(e.to)]){
                    dp[static_cast<std::size_t>(e.to)] = nd;
                    updated = true;
                }
            }
        }
        if(!updated) break;
    }
    return dp[static_cast<std::size_t>(t)];
}

void self_test(){
    {
        std::vector<std::vector<DijkstraEdge<long long>>> graph(4);
        graph[0].push_back({1, 3});
        graph[0].push_back({2, 10});
        graph[1].push_back({2, 4});
        graph[2].push_back({3, 2});
        auto res = dijkstra<long long>(graph, 0);
        assert(res.dist[3] == 9);
        assert(res.parent[3] == 2);
    }

    std::mt19937 rng(20260727);
    for(int n = 1; n <= 30; n++){
        for(int step = 0; step < 100; step++){
            std::vector<std::vector<DijkstraEdge<long long>>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(rng() % 5 == 0){
                        graph[static_cast<std::size_t>(u)].push_back({v, static_cast<int>(rng() % 20)});
                    }
                }
            }
            int s = static_cast<int>(rng() % n);
            auto res = dijkstra<long long>(graph, s, 1LL << 60);
            for(int v = 0; v < n; v++){
                assert(res.dist[static_cast<std::size_t>(v)] == brute_force_dist(graph, s, v));
            }
        }
    }
}

int main(){
    int n, m, s;
    if(!(std::cin >> n >> m >> s)){
        self_test();
        return 0;
    }
    std::vector<std::vector<DijkstraEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[static_cast<std::size_t>(u)].push_back({v, w});
    }
    constexpr long long INF = (1LL << 60);
    auto res = dijkstra<long long>(graph, s, INF);
    for(int i = 0; i < n; i++){
        if(i) std::cout << ' ';
        long long d = res.dist[static_cast<std::size_t>(i)];
        std::cout << (d == INF ? -1 : d);
    }
    std::cout << '\n';
}
