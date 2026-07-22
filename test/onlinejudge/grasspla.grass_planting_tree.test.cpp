// competitive-verifier: PROBLEM https://www.spoj.com/problems/GRASSPLA/

#include <iostream>

#include "../../src/algorithm/tree/construction/path_add_edge_get_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    PathAddEdgeGetTree<int, 100000> tree(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        tree.add_edge(left - 1, right - 1);
    }
    tree.build();

    while(query_count-- != 0){
        char operation;
        int left, right;
        std::cin >> operation >> left >> right;
        --left;
        --right;
        if(operation == 'P'){
            tree.path_add(left, right, 1);
        }else{
            std::cout << tree.edge_get(left, right) << '\n';
        }
    }
}
