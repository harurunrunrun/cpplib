// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/tetration_mod

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/tetration_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::uint64_t base, height;
        std::uint32_t modulus;
        std::cin >> base >> height >> modulus;
        std::cout << tetration_mod(base, height, modulus) << '\n';
    }
}
