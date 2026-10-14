// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/vertex_get_range_contour_add_on_tree

#include <iostream>
#include <tuple>
#include <vector>
#include "../../src/structure/tree/centroid/tree_contour_fenwick.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<long long> a(n); for(auto& x: a) std::cin >> x;
    std::vector<std::tuple<int, int, long long>> edges;
    for(int i = 1, u, v; i < n; ++i){ std::cin >> u >> v; edges.emplace_back(u, v, 1); }
    TreeContourFenwick<long long, long long, 100'000, true> tree(a, edges);
    while(q--){
        int t, v; std::cin >> t >> v;
        if(t == 0){ long long l, r, x; std::cin >> l >> r >> x; tree.contour_add(v, l, r, x); }
        else std::cout << tree.point_get(v) << '\n';
    }
}
