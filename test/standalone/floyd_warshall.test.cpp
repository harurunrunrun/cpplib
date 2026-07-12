// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/floyd_warshall.hpp"

constexpr long long INF = (1LL << 60);

std::vector<long long> bellman_ford(
    int n,
    const std::vector<std::tuple<int, int, long long>>& edges,
    int s
){
    std::vector<long long> dist(static_cast<std::size_t>(n), INF);
    dist[static_cast<std::size_t>(s)] = 0;
    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(auto [u, v, w]: edges){
            if(dist[static_cast<std::size_t>(u)] == INF) continue;
            long long nd = dist[static_cast<std::size_t>(u)] + w;
            if(nd < dist[static_cast<std::size_t>(v)]){
                dist[static_cast<std::size_t>(v)] = nd;
                updated = true;
            }
        }
        if(!updated) break;
    }
    return dist;
}

void self_test(){
    {
        std::vector<std::vector<long long>> dist(4, std::vector<long long>(4, INF));
        dist[0][1] = 3;
        dist[0][2] = 10;
        dist[1][2] = -2;
        dist[2][3] = 4;
        auto res = floyd_warshall(dist, INF);
        assert(res.dist[0][3] == 5);
        assert((res.path(0, 3) == std::vector<int>{0, 1, 2, 3}));
        assert(!res.has_negative_cycle);
    }
    for(int n = 1; n <= 11; n++){
        std::vector<std::tuple<int, int, long long>> edges;
        std::vector<std::vector<long long>> dist(static_cast<std::size_t>(n), std::vector<long long>(static_cast<std::size_t>(n), INF));
        for(int u = 0; u < n; u++){
            for(int v = u + 1; v < n; v++){
                long long w = (u * 17 + v * 31) % 13 - 5;
                edges.push_back({u, v, w});
                dist[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)] = w;
            }
        }
        auto res = floyd_warshall(dist, INF);
        for(int s = 0; s < n; s++){
            auto expected = bellman_ford(n, edges, s);
            assert(res.dist[static_cast<std::size_t>(s)] == expected);
        }
    }
    {
        std::vector<std::vector<long long>> dist(2, std::vector<long long>(2, INF));
        dist[0][1] = -1;
        dist[1][0] = -1;
        auto res = floyd_warshall(dist, INF);
        assert(res.has_negative_cycle);
        assert(res.negative[0] && res.negative[1]);
    }
}

int main(){
    int n, m;
    if(!(std::cin >> n >> m)){
        self_test();
        return 0;
    }
    std::vector<std::vector<long long>> dist(static_cast<std::size_t>(n), std::vector<long long>(static_cast<std::size_t>(n), INF));
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        if(w < dist[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)]){
            dist[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)] = w;
        }
    }
    auto res = floyd_warshall(dist, INF);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(j) std::cout << ' ';
            long long d = res.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)];
            if(d == INF) std::cout << "INF";
            else std::cout << d;
        }
        std::cout << '\n';
    }
}
