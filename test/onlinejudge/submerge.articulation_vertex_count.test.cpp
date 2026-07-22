// competitive-verifier: PROBLEM https://www.spoj.com/problems/SUBMERGE/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/articulation_vertex_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    for(int size, edge_count;
        std::cin >> size >> edge_count && (size != 0 || edge_count != 0);){
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            edges.emplace_back(left - 1, right - 1);
        }
        std::cout << articulation_vertex_count(size, edges) << '\n';
    }
}
