// competitive-verifier: PROBLEM https://www.spoj.com/problems/GCD2/
#include <cstdint>
#include <iostream>
#include <string>

#include "../../src/algorithm/math/number_theory/gcd_small_huge_integer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint32_t small;
        std::string huge;
        std::cin >> small >> huge;
        std::cout << math::gcd_small_huge_integer(small, huge) << '\n';
    }
}
