// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_rectangle_add_rectangle_sum

#include <iostream>
#include <vector>
#include "../../src/algorithm/range/offline_query/static_rectangle_integral.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<WeightedRectangle<Modint998244353>> rectangles(n);
    for(auto& r: rectangles) std::cin >> r.left >> r.lower >> r.right >> r.upper >> r.weight;
    std::vector<RectangleIntegralQuery> queries(q);
    for(auto& r: queries) std::cin >> r.left >> r.lower >> r.right >> r.upper;
    for(auto x: static_rectangle_integral(rectangles, queries)) std::cout << x << '\n';
}
