// competitive-verifier: PROBLEM https://www.spoj.com/problems/TREEISO/

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/tree_isomorphism.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count;
        std::cin >> vertex_count;
        std::vector<std::vector<int>> first(static_cast<std::size_t>(vertex_count));
        std::vector<std::vector<int>> second(static_cast<std::size_t>(vertex_count));

        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            --left;
            --right;
            first[static_cast<std::size_t>(left)].push_back(right);
            first[static_cast<std::size_t>(right)].push_back(left);
        }
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            --left;
            --right;
            second[static_cast<std::size_t>(left)].push_back(right);
            second[static_cast<std::size_t>(right)].push_back(left);
        }

        std::cout << (tree_isomorphic(first, second) ? "YES" : "NO") << '\n';
    }
}
