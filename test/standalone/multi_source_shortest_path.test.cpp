// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/graph/multi_source_shortest_path.hpp"

constexpr long long INF = (1LL << 60);

std::vector<long long> brute(
    const std::vector<std::vector<MultiSourceShortestPathEdge<long long>>>& graph,
    const std::vector<int>& sources
){
    const int n = static_cast<int>(graph.size());
    std::vector<long long> dist(static_cast<std::size_t>(n), INF);
    for(int s: sources) dist[static_cast<std::size_t>(s)] = 0;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(int v = 0; v < n; v++){
            if(dist[static_cast<std::size_t>(v)] == INF) continue;
            for(auto e: graph[static_cast<std::size_t>(v)]){
                long long nd = dist[static_cast<std::size_t>(v)] + e.cost;
                if(nd < dist[static_cast<std::size_t>(e.to)]){
                    dist[static_cast<std::size_t>(e.to)] = nd;
                    updated = true;
                }
            }
        }
        if(!updated) break;
    }
    return dist;
}

void self_test(){
    {
        std::vector<std::vector<MultiSourceShortestPathEdge<long long>>> graph(5);
        graph[0].push_back({1, 10});
        graph[2].push_back({1, 3});
        graph[1].push_back({3, 4});
        graph[4].push_back({3, 1});
        auto res = multi_source_shortest_path<long long>(graph, {0, 2, 4}, INF);
        assert(res.dist[1] == 3);
        assert(res.source[1] == 2);
        assert(res.dist[3] == 1);
        assert(res.source[3] == 4);
    }
    std::mt19937 rng(20260801);
    for(int n = 1; n <= 30; n++){
        for(int step = 0; step < 100; step++){
            std::vector<std::vector<MultiSourceShortestPathEdge<long long>>> graph(static_cast<std::size_t>(n));
            for(int u = 0; u < n; u++){
                for(int v = 0; v < n; v++){
                    if(rng() % 5 == 0){
                        graph[static_cast<std::size_t>(u)].push_back({v, static_cast<int>(rng() % 20)});
                    }
                }
            }
            std::vector<int> sources;
            for(int v = 0; v < n; v++){
                if(rng() % 4 == 0) sources.push_back(v);
            }
            auto res = multi_source_shortest_path<long long>(graph, sources, INF);
            assert(res.dist == brute(graph, sources));
        }
    }
}

int main(){
    int n, m, k;
    if(!(std::cin >> n >> m >> k)){
        self_test();
        return 0;
    }
    std::vector<int> sources(static_cast<std::size_t>(k));
    for(int i = 0; i < k; i++) std::cin >> sources[static_cast<std::size_t>(i)];
    std::vector<std::vector<MultiSourceShortestPathEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[static_cast<std::size_t>(u)].push_back({v, w});
    }
    auto res = multi_source_shortest_path<long long>(graph, sources, INF);
    for(int i = 0; i < n; i++){
        if(i) std::cout << ' ';
        long long d = res.dist[static_cast<std::size_t>(i)];
        std::cout << (d == INF ? -1 : d);
    }
    std::cout << '\n';
}
