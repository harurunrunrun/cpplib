// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/primitive_root

#include <iostream>

#include "../../src/algorithm/math/modular_arithmetic.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        unsigned long long prime;
        std::cin >> prime;
        std::cout << math::primitive_root(prime) << '\n';
    }
}
