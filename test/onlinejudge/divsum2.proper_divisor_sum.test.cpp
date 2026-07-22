// competitive-verifier: PROBLEM https://www.spoj.com/problems/DIVSUM2/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/proper_divisor_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t n;
        std::cin >> n;
        std::cout << math::proper_divisor_sum(n) << '\n';
    }
}
