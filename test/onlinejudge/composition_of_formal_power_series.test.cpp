// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/composition_of_formal_power_series
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_composition.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    math::Polynomial<998244353> a(n),b(n);
    for(auto& x:a) std::cin >> x;
    for(auto& x:b) std::cin >> x;
    const auto c=math::polynomial_compose<998244353>(a,b,n);
    for(int i=0;i<n;++i) std::cout << c[i] << (i+1==n?'\n':' ');
}
