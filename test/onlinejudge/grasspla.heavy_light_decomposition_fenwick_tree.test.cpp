// competitive-verifier: PROBLEM https://www.spoj.com/problems/GRASSPLA/

#include <algorithm>
#include <iostream>

#include "../../src/algorithm/tree/heavy_light_decomposition.hpp"
#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, operation_count;
    std::cin >> vertex_count >> operation_count;

    HeavyLightDecomposition decomposition(vertex_count);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        decomposition.add_edge(left - 1, right - 1);
    }
    decomposition.build();

    FenwickTree<int, 100001> difference(vertex_count + 1);
    while(operation_count-- != 0){
        char operation;
        int left, right;
        std::cin >> operation >> left >> right;
        --left;
        --right;

        if(operation == 'P'){
            for(const auto [range_left, range_right]:
                decomposition.path_query(left, right, false)){
                difference.add(range_left, 1);
                difference.add(range_right, -1);
            }
        }else{
            const int child = decomposition.depth(left) > decomposition.depth(right)
                ? left
                : right;
            std::cout << difference.prefix_sum(decomposition.in(child) + 1) << '\n';
        }
    }
}
