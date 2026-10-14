// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_point_set_rectangle_affine_rectangle_sum

#include <array>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include "../../src/structure/spatial/offline_indexed_rectangle_affine_sum.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    using mint = Modint998244353;
    int n, q; std::cin >> n >> q;
    std::vector<std::pair<std::int64_t, std::int64_t>> coordinates;
    std::vector<mint> initial(n);
    for(int i = 0; i < n; ++i){ std::int64_t x, y; std::cin >> x >> y >> initial[i]; coordinates.emplace_back(x, y); }
    std::vector<std::array<std::int64_t, 7>> queries(q);
    for(auto& a: queries){
        std::cin >> a[0];
        int count = a[0] == 0 ? 3 : a[0] == 1 ? 2 : a[0] == 2 ? 4 : 6;
        for(int i = 1; i <= count; ++i) std::cin >> a[i];
        if(a[0] == 0) coordinates.emplace_back(a[1], a[2]);
    }
    OfflineIndexedRectangleAffineSum<mint> tree(coordinates);
    for(int i = 0; i < n; ++i) tree.point_set(i, initial[i]);
    int next = n;
    for(const auto& a: queries){
        if(a[0] == 0) tree.point_set(next++, mint(a[3]));
        else if(a[0] == 1) tree.point_set(a[1], mint(a[2]));
        else if(a[0] == 2) std::cout << tree.rectangle_sum(a[1], a[2], a[3], a[4]) << '\n';
        else tree.rectangle_affine(a[1], a[2], a[3], a[4], mint(a[5]), mint(a[6]));
    }
}
