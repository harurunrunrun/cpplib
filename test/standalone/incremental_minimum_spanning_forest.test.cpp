// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>

#include "../../src/structure/graph/spanning_forest/incremental_minimum_spanning_forest.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 2;
    while(case_count--){
        int vertex_count, edge_count;
        std::cin >> vertex_count >> edge_count;
        IncrementalMinimumSpanningForest<long long, 256, 4096> forest(
            vertex_count
        );
        for(int edge = 0; edge < edge_count; ++edge){
            int first, second;
            long long weight;
            std::cin >> first >> second >> weight;
            if(edge) std::cout << ' ';
            std::cout << forest.add_edge(first, second, weight);
        }
        std::cout << '\n';
        if(forest.edge_count() != edge_count){
            throw std::runtime_error("incremental MSF edge count mismatch");
        }
    }
}
