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
        std::size_t order;
        std::cin >> order;
        const auto first =
            math::signed_stirling_numbers_first_kind<998244353>(order);
        const auto second =
            math::stirling_numbers_second_kind<998244353>(order);
        for(const auto& row: {first, second}){
            for(std::size_t index = 0; index < row.size(); ++index){
                if(index != 0) std::cout << ' ';
                std::cout << row[index];
            }
            std::cout << '\n';
        }
    }
}
