// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/graph/dijkstra.hpp"

struct BruteDistance{
    bool reachable;
    long long value;
};

BruteDistance brute_force_dist(
    const std::vector<std::vector<DijkstraEdge<long long>>>& graph,
    int s,
    int t
){
    const int n = static_cast<int>(graph.size());
    std::vector<long long> dist(static_cast<std::size_t>(n));
    std::vector<char> reachable(static_cast<std::size_t>(n), 0);
    dist[static_cast<std::size_t>(s)] = 0;
    reachable[static_cast<std::size_t>(s)] = 1;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(int v = 0; v < n; v++){
            if(!reachable[static_cast<std::size_t>(v)]) continue;
            for(auto e: graph[static_cast<std::size_t>(v)]){
                long long nd = dist[static_cast<std::size_t>(v)] + e.cost;
                if(!reachable[static_cast<std::size_t>(e.to)] ||
                   nd < dist[static_cast<std::size_t>(e.to)]){
                    dist[static_cast<std::size_t>(e.to)] = nd;
                    reachable[static_cast<std::size_t>(e.to)] = 1;
                    updated = true;
                }
            }
        }
        if(!updated) break;
    }
    return {static_cast<bool>(reachable[static_cast<std::size_t>(t)]),
            dist[static_cast<std::size_t>(t)]};
}

void self_test(){
    constexpr long long INF = (1LL << 60);
    {
        std::vector<std::vector<DijkstraEdge<long long>>> graph(4);
        graph[0].push_back({1, 3});
        graph[0].push_back({2, 10});
        graph[1].push_back({2, 4});
        graph[2].push_back({3, 2});
        auto res = dijkstra<long long>(graph, 0);
        assert(res.reachable[3]);
        assert(res.dist[3] == 9);
        assert(res.parent[3] == 2);
    }
    {
        std::vector<std::vector<DijkstraEdge<long long>>> graph(4);
        graph[0].push_back({1, INF});
        graph[1].push_back({2, 2000000000000000000LL});
        auto res = dijkstra<long long>(graph, 0, INF);
        assert(res.reachable[1] && res.dist[1] == INF);
        assert(res.reachable[2] && res.dist[2] == INF + 2000000000000000000LL);
        assert(!res.reachable[3] && res.dist[3] == INF);
    }
    {
        std::vector<std::vector<DijkstraEdge<long long>>> graph(2);
        graph[1].push_back({1, -1});
        bool thrown = false;
        try{
            (void)dijkstra<long long>(graph, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
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
            auto res = dijkstra<long long>(graph, s, INF);
            for(int v = 0; v < n; v++){
                auto expected = brute_force_dist(graph, s, v);
                assert(static_cast<bool>(res.reachable[static_cast<std::size_t>(v)]) == expected.reachable);
                if(expected.reachable){
                    assert(res.dist[static_cast<std::size_t>(v)] == expected.value);
                }
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
        if(res.reachable[static_cast<std::size_t>(i)]){
            std::cout << res.dist[static_cast<std::size_t>(i)];
        }else{
            std::cout << -1;
        }
    }
    std::cout << '\n';
}
