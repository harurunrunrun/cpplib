// competitive-verifier: PROBLEM https://www.spoj.com/problems/REC/
#include <cstdint>
#include <iostream>
#include <string>

#include "../../src/algorithm/math/affine_recurrence_term.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::string a, b, n;
        std::uint64_t modulus;
        std::cin >> a >> b >> n >> modulus;
        std::cout << math::affine_recurrence_term(a, b, n, modulus) << '\n';
    }
}
