// competitive-verifier: PROBLEM https://www.spoj.com/problems/MTOTALF/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/character_labeled_undirected_max_flow.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int edge_count;
    if(!(std::cin >> edge_count)) return 0;
    std::vector<CharacterLabeledUndirectedFlowEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    for(int index = 0; index < edge_count; ++index){
        char from, to;
        long long capacity;
        std::cin >> from >> to >> capacity;
        edges.push_back({from, to, capacity});
    }
    std::cout << character_labeled_undirected_max_flow(
        edges, 'A', 'Z'
    ) << '\n';
}
