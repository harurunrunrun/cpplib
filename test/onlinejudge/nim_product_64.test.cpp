// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/nim_product_64

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/arithmetic/nim_product_64.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::uint64_t left;
        std::uint64_t right;
        std::cin >> left >> right;
        std::cout << math::nim_product_64(left, right) << '\n';
    }
}
