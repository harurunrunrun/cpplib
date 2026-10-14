// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum_fixed_root

#include <iostream>
#include <vector>
#include "../../src/structure/tree/query/dynamic_affine_tree_path_sums.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    using mint = Modint998244353;
    int n, q; std::cin >> n >> q;
    std::vector<mint> a(n); for(auto& x: a) std::cin >> x;
    std::vector<AffineTreeEdge<mint>> edges(n-1); for(auto& e: edges) std::cin >> e.u >> e.v >> e.multiplier >> e.addend;
    DynamicAffineTreePathSums<mint> tree(a,edges);
    while(q--){
        int t, id; mint x, y; std::cin >> t >> id >> x;
        if(t == 0) tree.vertex_set(id,x);
        else{ std::cin >> y; tree.edge_set(id,x,y); }
        std::cout << tree.path_sum() << '\n';
    }
}
