// competitive-verifier: PROBLEM https://www.spoj.com/problems/SALMAN/

#include <iostream>

#include "../../src/structure/tree/subtree_add_sum_minimum_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int test_index = 1; test_index <= test_count; ++test_index){
        int vertex_count, query_count;
        std::cin >> vertex_count >> query_count;
        SubtreeAddSumMinimumTree<400000> tree(vertex_count);
        int root = -1;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            int parent;
            long long value;
            std::cin >> parent >> value;
            tree.set_value(vertex, value);
            if(parent == 0){
                root = vertex;
            }else{
                tree.add_edge(vertex, parent - 1);
            }
        }
        tree.build(root);

        std::cout << "Case " << test_index << ":\n";
        while(query_count-- != 0){
            int type, vertex;
            std::cin >> type >> vertex;
            --vertex;
            if(type == 1){
                std::cout << tree.subtree_sum(vertex) << '\n';
            }else{
                (void)tree.add_subtree_by_capped_minimum(vertex, 1000);
            }
        }
    }
}
