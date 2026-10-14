// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/inv_of_polynomials
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_inverse_mod.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,m; std::cin >> n >> m;
    math::Polynomial<998244353> a(n),b(m);
    for(auto& x:a) std::cin >> x;
    for(auto& x:b) std::cin >> x;
    const auto result=math::polynomial_inverse_mod<998244353>(a,b);
    if(!result){ std::cout << -1 << '\n'; return 0; }
    std::cout << result->size() << '\n';
    for(std::size_t i=0;i<result->size();++i)
        std::cout << (*result)[i] << (i+1==result->size()?'\n':' ');
    if(result->empty()) std::cout << '\n';
}
