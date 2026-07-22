// competitive-verifier: PROBLEM https://www.spoj.com/problems/TREEISO/

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/isomorphism/are_unrooted_trees_isomorphic.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests-- != 0){
        int n;
        std::cin >> n;
        std::vector<std::vector<int>> first(static_cast<std::size_t>(n));
        std::vector<std::vector<int>> second(static_cast<std::size_t>(n));
        for(auto* tree: {&first, &second}){
            for(int edge = 1; edge < n; ++edge){
                int u, v;
                std::cin >> u >> v;
                --u;
                --v;
                (*tree)[static_cast<std::size_t>(u)].push_back(v);
                (*tree)[static_cast<std::size_t>(v)].push_back(u);
            }
        }
        std::cout << (are_unrooted_trees_isomorphic(first, second) ? "YES\n" : "NO\n");
    }
}
