// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_inversions_query

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/range/offline_query/range_inversion_queries.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<int> a(n); for(int& x: a) std::cin >> x;
    std::vector<std::pair<int, int>> ranges(q); for(auto& [l, r]: ranges) std::cin >> l >> r;
    for(auto x: range_inversion_queries(a, ranges)) std::cout << x << '\n';
}
