// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_range_affine_range_sum

#include <iostream>
#include <vector>
#include "../../src/structure/segtree/persistent/persistent_range_affine_sum.hpp"
#include "../../src/structure/modint/modint.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<Modint998244353> a(n); for(auto& x: a) std::cin >> x;
    PersistentRangeAffineSum<Modint998244353> tree(a);
    std::vector<int> version(q + 1); // version[k+1] represents sequence A[k].
    for(int i = 0; i < q; ++i){
        int t, k, l, r; std::cin >> t >> k;
        if(t == 0){ Modint998244353 b, c; std::cin >> l >> r >> b >> c; version[i + 1] = tree.affine(version[k + 1], l, r, b, c); }
        else if(t == 1){ int s; std::cin >> s >> l >> r; version[i + 1] = tree.copy_range(version[k + 1], version[s + 1], l, r); }
        else{ std::cin >> l >> r; std::cout << tree.range_sum(version[k + 1], l, r) << '\n'; }
    }
}
