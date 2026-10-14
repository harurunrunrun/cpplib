// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/consecutive_terms_of_linear_recurrent_sequence
#include <cstdint>
#include <iostream>
#include "../../src/algorithm/math/sequence/linear_recurrence_range.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int d,m; std::uint64_t k; std::cin >> d >> k >> m;
    math::Polynomial<998244353> a(d),c(d);
    for(auto& x:a) std::cin >> x;
    for(auto& x:c) std::cin >> x;
    const auto result=math::linear_recurrence_range<998244353>(a,c,k,m);
    for(int i=0;i<m;++i) std::cout << result[i] << (i+1==m?'\n':' ');
}
