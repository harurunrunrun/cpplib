// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/stirling_number_of_the_first_kind_fixed_k

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/stirling_numbers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t maximum_order;
    std::size_t fixed_cycles;
    std::cin >> maximum_order >> fixed_cycles;
    const auto answer =
        math::signed_stirling_numbers_first_kind_fixed_k<998244353>(
            maximum_order, fixed_cycles
        );
    for(std::size_t order = fixed_cycles; order <= maximum_order; ++order){
        if(order != fixed_cycles) std::cout << ' ';
        std::cout << answer[order];
    }
    std::cout << '\n';
}
