// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/flow_cut/character_labeled_undirected_max_flow.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int edge_count;
        char source, sink;
        std::cin >> edge_count >> source >> sink;
        std::vector<CharacterLabeledUndirectedFlowEdge<long long>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            char from, to;
            long long capacity;
            std::cin >> from >> to >> capacity;
            edges.push_back({from, to, capacity});
        }
        std::cout << character_labeled_undirected_max_flow(
            edges, source, sink
        ) << '\n';
    }
}
