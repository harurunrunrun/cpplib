// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/common_interval_decomposition_tree

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/construction/permutation_interval_tree.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n; std::vector<int> p(n); for(int& x: p) std::cin >> x;
    PermutationIntervalTree tree(p); const auto& nodes = tree.nodes();
    std::vector<int> parent(nodes.size(), -1);
    for(int i = 0; i < static_cast<int>(nodes.size()); ++i) for(int child: nodes[i].children) parent[child] = i;
    std::cout << nodes.size() << '\n';
    for(int i = 0; i < static_cast<int>(nodes.size()); ++i) std::cout << parent[i] << ' ' << nodes[i].left << ' ' << nodes[i].right - 1 << ' ' << (nodes[i].linear ? "linear" : "prime") << '\n';
}
