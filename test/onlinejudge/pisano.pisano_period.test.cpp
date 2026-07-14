// competitive-verifier: PROBLEM https://www.spoj.com/problems/PISANO/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/pisano_period.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t modulus;
        std::cin >> modulus;
        std::cout << math::pisano_period(modulus) << '\n';
    }
}
