#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_1_A"

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/dijkstra.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count, source;
    std::cin >> vertex_count >> edge_count >> source;
    std::vector<std::vector<DijkstraEdge<long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
    }
    const auto result = dijkstra(graph, source);
    for(int vertex = 0; vertex < vertex_count; vertex++){
        if(result.reachable[static_cast<std::size_t>(vertex)]){
            std::cout << result.dist[static_cast<std::size_t>(vertex)] << '\n';
        }else{
            std::cout << "INF\n";
        }
    }
}
