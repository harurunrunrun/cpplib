// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_tree_vertex_set_path_composite

#include <iostream>
#include <vector>
#include "../../src/structure/tree/dynamic_forest/link_cut_tree.hpp"
#include "../../src/structure/types/monoid/common_monoids.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;
constexpr AffineCompositionMonoid<mint> composition;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<MonoidAffine<mint>> values(n);
    for(auto& f: values) std::cin >> f.multiplier >> f.addend;
    LinkCutTree<composition, 200'000> tree(values);
    for(int i = 1, u, v; i < n; ++i){ std::cin >> u >> v; tree.link(u, v); }
    while(q--){
        int type; std::cin >> type;
        if(type == 0){ int u, v, w, x; std::cin >> u >> v >> w >> x; tree.cut(u, v); tree.link(w, x); }
        else if(type == 1){ int p; mint a, b; std::cin >> p >> a >> b; tree.set(p, {a, b}); }
        else{ int u, v; mint x; std::cin >> u >> v >> x; const auto f = tree.path_prod(u, v); std::cout << f.multiplier * x + f.addend << '\n'; }
    }
}
