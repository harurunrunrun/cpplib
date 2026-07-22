// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/combinatorics/bernoulli_number.hpp"

int main(){
    try{
        (void)bernoulli_numbers<7>(6);
        return 1;
    }catch(const std::length_error&){
    }
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::size_t maximum_index;
        std::cin >> maximum_index;
        const auto answer = bernoulli_numbers<998244353>(maximum_index);
        for(std::size_t index = 0; index < answer.size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << answer[index];
        }
        std::cout << '\n';
    }
}
