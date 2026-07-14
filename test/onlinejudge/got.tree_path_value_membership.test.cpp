// competitive-verifier: PROBLEM https://www.spoj.com/problems/GOT/

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/tree_path_value_membership.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    while(std::cin >> n >> q){
        std::vector<int> values(static_cast<std::size_t>(n));
        for(int& value: values) std::cin >> value;
        TreePathValueMembership<int, 100000> tree(n);
        for(int edge = 1; edge < n; ++edge){
            int u, v;
            std::cin >> u >> v;
            tree.add_edge(u - 1, v - 1);
        }
        tree.build(values);
        while(q-- != 0){
            int u, v, value;
            std::cin >> u >> v >> value;
            std::cout << (tree.contains(u - 1, v - 1, value) ? "Find\n" : "NotFind\n");
        }
        std::cout << '\n';
    }
}
