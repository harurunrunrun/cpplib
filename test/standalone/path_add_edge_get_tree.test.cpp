// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/algorithm/tree/path_add_edge_get_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, operation_count, root;
        std::cin >> vertex_count >> operation_count >> root;
        PathAddEdgeGetTree<long long, 5000> tree(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            tree.add_edge(left, right);
        }
        tree.build(root);

        while(operation_count-- > 0){
            char operation;
            int left, right;
            std::cin >> operation >> left >> right;
            if(operation == 'A'){
                long long delta;
                std::cin >> delta;
                tree.path_add(left, right, delta);
            }else{
                std::cout << tree.edge_get(left, right) << '\n';
            }
        }
    }
}
