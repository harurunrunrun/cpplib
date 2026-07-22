// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_floor_of_linear

#include <iostream>

#include "../../src/algorithm/math/number_theory/floor_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        long long count, modulus, multiplier, addend;
        std::cin >> count >> modulus >> multiplier >> addend;
        std::cout << math::floor_sum(
            count, modulus, multiplier, addend
        ) << '\n';
    }
}
