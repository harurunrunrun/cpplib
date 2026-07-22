// competitive-verifier: PROBLEM https://www.spoj.com/problems/POWERPHI/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/sequence/nearest_golden_ratio_power.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t exponent;
        std::cin >> exponent;
        std::cout << math::nearest_golden_ratio_power_mod(exponent) << '\n';
    }
}
