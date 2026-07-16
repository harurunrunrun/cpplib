// competitive-verifier: PROBLEM https://www.spoj.com/problems/GCPC11J/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/unweighted_tree_radius.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int vertex_count;
        std::cin >> vertex_count;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(vertex_count - 1));
        for(int index = 1; index < vertex_count; index++){
            int left, right;
            std::cin >> left >> right;
            edges.emplace_back(left, right);
        }
        std::cout << unweighted_tree_radius(vertex_count, edges) << '\n';
    }
}
