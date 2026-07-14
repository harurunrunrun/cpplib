// competitive-verifier: PROBLEM https://www.spoj.com/problems/COT/

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/tree_path_kth_value.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    TreePathKthValue<int, 100000> tree(n);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    tree.build(values);
    while(q-- != 0){
        int u, v, order;
        std::cin >> u >> v >> order;
        std::cout << tree.kth_smallest(u - 1, v - 1, order - 1) << '\n';
    }
}
