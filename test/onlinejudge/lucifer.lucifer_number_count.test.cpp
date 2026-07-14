// competitive-verifier: PROBLEM https://www.spoj.com/problems/LUCIFER/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/lucifer_number_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t left, right;
        std::cin >> left >> right;
        std::cout << math::lucifer_number_count(left, right) << '\n';
    }
}
