// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/stirling_numbers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::size_t maximum_order;
        std::size_t fixed_index;
        std::cin >> maximum_order >> fixed_index;
        const auto first =
            math::signed_stirling_numbers_first_kind_fixed_k<998244353>(
                maximum_order, fixed_index
            );
        const auto second =
            math::stirling_numbers_second_kind_fixed_k<998244353>(
                maximum_order, fixed_index
            );
        for(const auto& column: {first, second}){
            for(std::size_t order = 0; order <= maximum_order; ++order){
                if(order != 0) std::cout << ' ';
                std::cout << column[order];
            }
            std::cout << '\n';
        }
    }
}
