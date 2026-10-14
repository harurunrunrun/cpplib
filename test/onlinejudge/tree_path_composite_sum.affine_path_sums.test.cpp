// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/tree_path_composite_sum

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/dynamic_programming/affine_path_sums.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    std::vector<Modint998244353> values(n); for(auto& x: values) std::cin >> x;
    std::vector<AffineTreeEdge<Modint998244353>> edges(n - 1);
    for(auto& e: edges) std::cin >> e.u >> e.v >> e.multiplier >> e.addend;
    auto answer = affine_path_sums(values, edges);
    for(int i = 0; i < n; ++i) std::cout << answer[i] << (i + 1 == n ? '\n' : ' ');
}
