// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/stirling_number_of_the_second_kind_fixed_k

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/stirling_numbers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t maximum_order;
    std::size_t fixed_parts;
    std::cin >> maximum_order >> fixed_parts;
    const auto answer =
        math::stirling_numbers_second_kind_fixed_k<998244353>(
            maximum_order, fixed_parts
        );
    for(std::size_t order = fixed_parts; order <= maximum_order; ++order){
        if(order != fixed_parts) std::cout << ' ';
        std::cout << answer[order];
    }
    std::cout << '\n';
}
