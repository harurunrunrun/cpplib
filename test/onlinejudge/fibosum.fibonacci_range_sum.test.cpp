// competitive-verifier: PROBLEM https://www.spoj.com/problems/FIBOSUM/
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/sequence/fibonacci_range_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests--){
        std::uint64_t left, right;
        std::cin >> left >> right;
        std::cout << math::fibonacci_range_sum(left, right) << '\n';
    }
}
