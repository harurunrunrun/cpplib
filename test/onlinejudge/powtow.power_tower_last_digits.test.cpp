// competitive-verifier: PROBLEM https://www.spoj.com/problems/POWTOW/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/power_tower_last_digits.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t base, height;
        std::cin >> base >> height;
        std::cout << math::power_tower_last_nine(base, height).to_string() << '\n';
    }
}
