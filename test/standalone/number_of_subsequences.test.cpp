// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/combinatorics/number_of_subsequences.hpp"

int main(){
    try{
        (void)number_of_distinct_subsequences(std::vector<int>{1}, 0);
        return 1;
    }catch(const std::invalid_argument&){
    }
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::size_t size;
        std::uint32_t modulus;
        std::cin >> size >> modulus;
        std::vector<long long> values(size);
        for(long long& value: values) std::cin >> value;
        std::cout << number_of_distinct_subsequences(values, modulus) << '\n';
    }
}
