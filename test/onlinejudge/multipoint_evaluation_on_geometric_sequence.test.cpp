// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/multipoint_evaluation_on_geometric_sequence
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_geometric_sequence.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,m; Modint998244353 a,r; std::cin >> n >> m >> a >> r;
    math::Polynomial<998244353> f(n);
    for(auto& x:f) std::cin >> x;
    const auto result=math::polynomial_evaluate_geometric<998244353>(f,a,r,m);
    for(int i=0;i<m;++i) std::cout << result[i] << (i+1==m?'\n':' ');
}
