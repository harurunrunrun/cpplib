// competitive-verifier: STANDALONE

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/tree/subtree_add_sum_minimum_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, query_count, root;
        std::cin >> vertex_count >> query_count >> root;
        SubtreeAddSumMinimumTree<5000> tree(vertex_count);
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            long long value;
            std::cin >> value;
            tree.set_value(vertex, value);
        }
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            tree.add_edge(left, right);
        }
        tree.build(root);

        while(query_count-- > 0){
            char operation;
            int vertex;
            std::cin >> operation >> vertex;
            if(operation == 'S'){
                std::cout << tree.subtree_sum(vertex) << '\n';
            }else if(operation == 'M'){
                std::cout << tree.subtree_minimum(vertex) << '\n';
            }else{
                long long value;
                std::cin >> value;
                if(operation == 'A'){
                    tree.subtree_add(vertex, value);
                }else{
                    std::cout << tree.add_subtree_by_capped_minimum(
                        vertex, value
                    ) << '\n';
                }
            }
        }
    }
}
