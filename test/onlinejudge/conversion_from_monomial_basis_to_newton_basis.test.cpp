// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/conversion_from_monomial_basis_to_newton_basis
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_newton_basis.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    math::Polynomial<998244353> a(n),p(n);
    for(auto& x:a) std::cin >> x;
    for(auto& x:p) std::cin >> x;
    const auto b=math::polynomial_to_newton_basis<998244353>(a,p);
    for(int i=0;i<n;++i) std::cout << b[i] << (i+1==n?'\n':' ');
    if(n==0) std::cout << '\n';
}
