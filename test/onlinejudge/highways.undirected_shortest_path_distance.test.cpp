// competitive-verifier: PROBLEM https://www.spoj.com/problems/HIGHWAYS/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/undirected_shortest_path_distance.hpp"

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
        std::vector<UndirectedShortestPathEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            --from;
            --to;
            edges.push_back({from, to, cost});
        }
        const auto answer = undirected_shortest_path_distance(
            vertex_count, edges, source, target
        );
        if(answer) std::cout << *answer << '\n';
        else std::cout << "NONE\n";
    }
}
