// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_1_C

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/floyd_warshall.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    constexpr long long inf = std::numeric_limits<long long>::max() / 4;
    std::vector<std::vector<long long>> dist(
        static_cast<std::size_t>(vertex_count),
        std::vector<long long>(static_cast<std::size_t>(vertex_count), inf)
    );
    for(int vertex = 0; vertex < vertex_count; vertex++){
        dist[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(vertex)] = 0;
    }
    while(edge_count--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        auto& current = dist[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)];
        current = std::min(current, cost);
    }
    const auto result = floyd_warshall(std::move(dist), inf);
    if(result.has_negative_cycle){
        std::cout << "NEGATIVE CYCLE\n";
        return 0;
    }
    for(int from = 0; from < vertex_count; from++){
        for(int to = 0; to < vertex_count; to++){
            if(to) std::cout << ' ';
            if(result.reachable[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)]){
                std::cout << result.dist[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)];
            }else{
                std::cout << "INF";
            }
        }
        std::cout << '\n';
    }
}
