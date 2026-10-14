// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/compositional_inverse_of_formal_power_series
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_reversion.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    math::Polynomial<998244353> a(n);
    for(auto& x:a) std::cin >> x;
    const auto b=math::polynomial_reversion<998244353>(a,n);
    for(int i=0;i<n;++i) std::cout << b[i] << (i+1==n?'\n':' ');
}
