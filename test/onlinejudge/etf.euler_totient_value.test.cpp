// competitive-verifier: PROBLEM https://www.spoj.com/problems/ETF/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/euler_totient_value.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t n;
        std::cin >> n;
        std::cout << math::euler_totient_value(n) << '\n';
    }
}
