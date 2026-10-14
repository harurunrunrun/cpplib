// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/unionfind_with_potential_non_commutative_group

#include <array>
#include <iostream>
#include "../../src/structure/dsu/basic/group_potential_dsu.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;
struct MatrixGroup{
    using value_type = std::array<mint, 4>;
    static value_type identity(){ return {1, 0, 0, 1}; }
    static value_type multiply(const value_type& a, const value_type& b){
        return {a[0]*b[0]+a[1]*b[2], a[0]*b[1]+a[1]*b[3],
                a[2]*b[0]+a[3]*b[2], a[2]*b[1]+a[3]*b[3]};
    }
    static value_type inverse(const value_type& a){ return {a[3], -a[1], -a[2], a[0]}; }
};
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    GroupPotentialDSU<MatrixGroup> dsu(n);
    while(q--){
        int t, u, v; std::cin >> t >> u >> v;
        if(t == 0){ MatrixGroup::value_type x; for(auto& a: x) std::cin >> a; std::cout << dsu.merge(u, v, x) << '\n'; }
        else{ auto x = dsu.difference(u, v); if(!x) std::cout << -1 << '\n'; else std::cout << (*x)[0] << ' ' << (*x)[1] << ' ' << (*x)[2] << ' ' << (*x)[3] << '\n'; }
    }
}
