// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/combinatorics/montmort_number.hpp"

int main(){
    try{
        (void)montmort_numbers(3, 0);
        return 1;
    }catch(const std::invalid_argument&){
    }
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::size_t maximum_size;
        std::uint32_t modulus;
        std::cin >> maximum_size >> modulus;
        const auto answer = montmort_numbers(maximum_size, modulus);
        for(std::size_t size = 0; size <= maximum_size; ++size){
            if(size != 0) std::cout << ' ';
            std::cout << answer[size];
        }
        std::cout << '\n';
    }
}
