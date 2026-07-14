// competitive-verifier: PROBLEM https://www.spoj.com/problems/PT07J/

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/subtree_label_order_statistics.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<int> labels(static_cast<std::size_t>(n));
    for(int& label: labels) std::cin >> label;
    SubtreeLabelOrderStatistics<100000> tree(labels);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        tree.add_edge(u - 1, v - 1);
    }
    tree.build();
    int q;
    std::cin >> q;
    while(q-- != 0){
        int root, order;
        std::cin >> root >> order;
        std::cout << tree.kth_smallest_vertex(root - 1, order - 1) + 1 << '\n';
    }
}
