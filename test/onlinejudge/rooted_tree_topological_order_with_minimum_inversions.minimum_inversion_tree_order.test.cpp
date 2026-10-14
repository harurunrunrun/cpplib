// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rooted_tree_topological_order_with_minimum_inversions

#include <cstdint>
#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/construction/minimum_inversion_tree_order.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    std::vector<int> parent(n, -1); for(int i = 1; i < n; ++i) std::cin >> parent[i];
    std::vector<std::int64_t> c(n), d(n); for(auto& x: c) std::cin >> x; for(auto& x: d) std::cin >> x;
    auto answer = minimum_inversion_tree_order(parent, c, d);
    std::cout << static_cast<long long>(answer.cost) << '\n';
    for(int i = 0; i < n; ++i) std::cout << answer.order[i] << (i + 1 == n ? '\n' : ' ');
}
