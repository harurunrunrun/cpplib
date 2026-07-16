// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE/

#include <iostream>
#include <string>

#include "../../src/structure/tree/tree_edge_point_set_path_maximum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count;
        std::cin >> vertex_count;
        TreeEdgePointSetPathMaximum<int, 10000> tree(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right, weight;
            std::cin >> left >> right >> weight;
            tree.add_edge(left - 1, right - 1, weight);
        }
        tree.build();

        std::string operation;
        while(std::cin >> operation && operation != "DONE"){
            int first, second;
            std::cin >> first >> second;
            if(operation == "QUERY"){
                std::cout << tree.path_maximum(first - 1, second - 1) << '\n';
            }else{
                tree.set_edge(first - 1, second);
            }
        }
    }
    return 0;
}
