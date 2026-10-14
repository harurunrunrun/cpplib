// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_root_finding
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_roots.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    math::Polynomial<998244353> a(n+1);
    for(auto& x:a) std::cin >> x;
    const auto roots=math::polynomial_roots<998244353>(a);
    std::cout << roots.size() << '\n';
    for(std::size_t i=0;i<roots.size();++i)
        std::cout << roots[i] << (i+1==roots.size()?'\n':' ');
    if(roots.empty()) std::cout << '\n';
}
