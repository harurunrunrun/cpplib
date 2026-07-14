// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/NTL_1_D

#include <iostream>

#include "../../src/algorithm/math/prime_factorization.hpp"

int main(){
    unsigned long long value;
    std::cin >> value;
    std::cout << math::euler_phi(value) << '\n';
}
