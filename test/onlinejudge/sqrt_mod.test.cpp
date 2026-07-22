// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sqrt_mod

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/sqrt_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        std::uint64_t value, prime;
        std::cin >> value >> prime;
        const auto root = math::sqrt_mod(value, prime);
        if(root) std::cout << *root << '\n';
        else std::cout << -1 << '\n';
    }
}
