// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rational_approximation
#include <cstdint>
#include <iostream>
#include "../../src/algorithm/math/number_theory/arithmetic/rational_bounds.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int t; std::cin >> t;
    while(t--){
        std::uint64_t n,x,y; std::cin >> n >> x >> y;
        const auto [lower,upper]=math::rational_bounds(x,y,n);
        std::cout << lower.first << ' ' << lower.second << ' '
                  << upper.first << ' ' << upper.second << '\n';
    }
}
