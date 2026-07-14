// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_1_B

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/bellman_ford.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count, source;
    std::cin >> vertex_count >> edge_count >> source;
    std::vector<BellmanFordEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }
    const auto result = bellman_ford(vertex_count, edges, source);
    if(result.has_negative_cycle){
        std::cout << "NEGATIVE CYCLE\n";
        return 0;
    }
    for(int vertex = 0; vertex < vertex_count; vertex++){
        if(result.reachable[static_cast<std::size_t>(vertex)]){
            std::cout << result.dist[static_cast<std::size_t>(vertex)] << '\n';
        }else{
            std::cout << "INF\n";
        }
    }
}
