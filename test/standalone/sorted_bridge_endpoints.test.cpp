// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/sorted_bridge_endpoints.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        edges.emplace_back(left, right);
    }
    const auto bridges = sorted_bridge_endpoints(vertex_count, edges);
    std::cout << bridges.size() << '\n';
    for(const auto [left, right]: bridges){
        std::cout << left << ' ' << right << '\n';
    }
}
