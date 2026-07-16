// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/dsu/offline_tree_edge_deletion_disconnected_pairs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count;
        std::cin >> vertex_count;
        OfflineTreeEdgeDeletionDisconnectedPairs<64> structure(vertex_count);
        for(int edge = 0; edge + 1 < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            structure.add_edge(left, right);
        }

        int operation_count;
        std::cin >> operation_count;
        while(operation_count-- > 0){
            char type;
            std::cin >> type;
            if(type == 'D'){
                int edge;
                std::cin >> edge;
                structure.delete_edge(edge);
            }else{
                structure.add_query();
            }
        }
        const auto answers = structure.solve();
        if(structure.solve() != answers) return 2;
        for(long long answer: answers) std::cout << answer << '\n';
    }
}
