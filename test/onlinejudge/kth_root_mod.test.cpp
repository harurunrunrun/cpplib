// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/kth_root_mod
#include <cstdint>
#include <iostream>
#include "../../src/algorithm/math/number_theory/modular/one_kth_root_mod_prime.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int t; std::cin >> t;
    while(t--){
        std::uint64_t k,y,p; std::cin >> k >> y >> p;
        const auto root=math::one_kth_root_mod_prime(y,k,p);
        if(root) std::cout << *root << '\n';else std::cout << -1 << '\n';
    }
}
