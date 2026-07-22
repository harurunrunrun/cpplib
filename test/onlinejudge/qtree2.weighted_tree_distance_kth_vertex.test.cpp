// competitive-verifier: PROBLEM https://www.spoj.com/problems/QTREE2/

#include <iostream>
#include <string>

#include "../../src/algorithm/tree/query/weighted_tree_distance_kth_vertex.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count;
        std::cin >> vertex_count;
        WeightedTreeDistanceKthVertex<long long, 10000> tree(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            long long weight;
            std::cin >> left >> right >> weight;
            tree.add_edge(left - 1, right - 1, weight);
        }
        tree.build();

        std::string operation;
        while(std::cin >> operation && operation != "DONE"){
            int left, right;
            std::cin >> left >> right;
            if(operation == "DIST"){
                std::cout << tree.distance(left - 1, right - 1) << '\n';
            }else{
                int order;
                std::cin >> order;
                std::cout
                    << tree.kth_vertex(left - 1, right - 1, order - 1) + 1
                    << '\n';
            }
        }
        std::cout << '\n';
    }
    return 0;
}
