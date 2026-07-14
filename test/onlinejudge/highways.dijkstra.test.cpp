// competitive-verifier: PROBLEM https://www.spoj.com/problems/HIGHWAYS/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/dijkstra.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        int vertex_count, edge_count, source, target;
        std::cin >> vertex_count >> edge_count >> source >> target;
        --source;
        --target;
        std::vector<std::vector<DijkstraEdge<long long>>> graph(
            static_cast<std::size_t>(vertex_count)
        );
        while(edge_count-- > 0){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            --from;
            --to;
            graph[static_cast<std::size_t>(from)].push_back({to, cost});
            graph[static_cast<std::size_t>(to)].push_back({from, cost});
        }
        const DijkstraResult<long long> result = dijkstra(graph, source);
        if(result.reachable[static_cast<std::size_t>(target)]){
            std::cout << result.dist[static_cast<std::size_t>(target)] << '\n';
        }else{
            std::cout << "NONE\n";
        }
    }
}
