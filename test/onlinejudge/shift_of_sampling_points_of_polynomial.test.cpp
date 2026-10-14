// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/shift_of_sampling_points_of_polynomial
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_geometric_sequence.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,m; Modint998244353 c; std::cin >> n >> m >> c;
    math::Polynomial<998244353> values(n);
    for(auto& x:values) std::cin >> x;
    const auto result=math::polynomial_shift_samples<998244353>(values,c,m);
    for(int i=0;i<m;++i) std::cout << result[i] << (i+1==m?'\n':' ');
}
