// competitive-verifier: PROBLEM https://www.spoj.com/problems/LUCIFER/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/count_prime_alternating_digit_sum_numbers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t left, right;
        std::cin >> left >> right;
        std::cout << math::count_prime_alternating_digit_sum_numbers(left, right) << '\n';
    }
}
