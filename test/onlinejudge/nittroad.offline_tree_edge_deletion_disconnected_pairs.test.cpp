// competitive-verifier: PROBLEM https://www.spoj.com/problems/NITTROAD/

#include <iostream>

#include "../../src/structure/dsu/offline_tree_edge_deletion_disconnected_pairs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int vertex_count;
        std::cin >> vertex_count;
        OfflineTreeEdgeDeletionDisconnectedPairs<20000> structure(
            vertex_count
        );
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            structure.add_edge(left - 1, right - 1);
        }
        int operation_count;
        std::cin >> operation_count;
        while(operation_count-- > 0){
            char type;
            std::cin >> type;
            if(type == 'Q'){
                structure.add_query();
            }else{
                int edge;
                std::cin >> edge;
                structure.delete_edge(edge - 1);
            }
        }
        for(long long answer: structure.solve()) std::cout << answer << '\n';
        std::cout << '\n';
    }
}
