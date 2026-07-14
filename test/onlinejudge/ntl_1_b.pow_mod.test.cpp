// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/NTL_1_B

#include <iostream>

#include "../../src/algorithm/math/basic_number_theory.hpp"

int main(){
    long long value, exponent;
    std::cin >> value >> exponent;
    std::cout << math::pow_mod(value, exponent, 1000000007) << '\n';
}
