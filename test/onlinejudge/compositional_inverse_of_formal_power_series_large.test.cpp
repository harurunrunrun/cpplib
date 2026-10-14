// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/compositional_inverse_of_formal_power_series_large
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_composition_fast.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int n;std::cin>>n;math::Polynomial<998244353> f(n);
    for(auto& x:f)std::cin>>x;
    const auto result=math::polynomial_reversion_fast<998244353>(f,n);
    for(int i=0;i<n;++i)std::cout<<result[i]<<(i+1==n?'\n':' ');
}
