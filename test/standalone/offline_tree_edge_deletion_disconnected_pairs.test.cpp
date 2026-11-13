// competitive-verifier: STANDALONE

#include <iostream>
#include <cassert>

#include <utility>
#include <vector>

#include "../../src/structure/dsu/specialized/offline_tree_edge_deletion_disconnected_pairs.hpp"

void check_large_fixed_capacity(){
    // Two fixed-capacity DSUs of this size would not fit on a typical 8 MiB stack.
    OfflineTreeEdgeDeletionDisconnectedPairs<4000000> structure(4);
    const int first = structure.add_edge(0, 1);
    structure.add_edge(1, 2);
    const int third = structure.add_edge(2, 3);
    structure.add_query();
    structure.delete_edge(first);
    structure.add_query();
    structure.delete_edge(third);
    structure.add_query();
    const std::vector<long long> expected{0, 3, 5};
    assert(structure.solve() == expected);
}

int main(){
    check_large_fixed_capacity();
    // Dynamic wrappers: runtime capacities.
    {
        OfflineTreeEdgeDeletionDisconnectedPairs<> tree(129);
        for(int i = 0; i < 128; ++i) tree.add_edge(i, i + 1);
        tree.add_query(); tree.delete_edge(63); tree.add_query();
        assert((tree.solve() == std::vector<long long>{0, 4160}));
    }
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
