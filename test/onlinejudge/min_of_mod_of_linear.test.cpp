// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/min_of_mod_of_linear

#include <iostream>

#include "../../src/algorithm/math/number_theory/min_of_mod_of_linear.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        long long count, modulus, coefficient, constant;
        std::cin >> count >> modulus >> coefficient >> constant;
        std::cout << math::min_of_mod_of_linear(
            count, modulus, coefficient, constant
        ) << '\n';
    }
}
