// competitive-verifier: PROBLEM https://www.spoj.com/problems/TOPOSORT/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/traversal/lexicographically_smallest_topological_order.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size, edge_count;
    std::cin >> size >> edge_count;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
    while(edge_count-- != 0){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from - 1)].push_back(to - 1);
    }
    const auto order = lexicographically_smallest_topological_order(graph);
    if(!order){
        std::cout << "Sandro fails.\n";
        return 0;
    }
    for(const int vertex: *order) std::cout << vertex + 1 << ' ';
    std::cout << '\n';
}
