// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/kth_root_integer

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/kth_root_integer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        std::uint64_t value;
        unsigned int exponent;
        std::cin >> value >> exponent;
        std::cout << math::kth_root_integer(value, exponent) << '\n';
    }
}
