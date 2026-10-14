// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_interpolation_on_geometric_sequence
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_geometric_sequence.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; Modint998244353 a,r; std::cin >> n >> a >> r;
    math::Polynomial<998244353> values(n);
    for(auto& x:values) std::cin >> x;
    const auto result=math::polynomial_interpolate_geometric<998244353>(values,a,r);
    for(int i=0;i<n;++i) std::cout << result[i] << (i+1==n?'\n':' ');
    if(n==0) std::cout << '\n';
}
