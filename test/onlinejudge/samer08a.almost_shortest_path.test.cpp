// competitive-verifier: PROBLEM https://www.spoj.com/problems/SAMER08A/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/almost_shortest_path.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    while(std::cin >> n >> m && (n != 0 || m != 0)){
        int source, target;
        std::cin >> source >> target;
        std::vector<AlmostShortestPathEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(m));
        for(int edge_index = 0; edge_index < m; edge_index++){
            int from, to;
            long long cost;
            std::cin >> from >> to >> cost;
            edges.push_back({from, to, cost});
        }
        auto result = almost_shortest_path<long long>(
            n, edges, source, target
        );
        std::cout << (result.reachable ? result.distance : -1) << '\n';
    }
}
