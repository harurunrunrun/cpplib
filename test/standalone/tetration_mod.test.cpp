// competitive-verifier: STANDALONE

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/tetration_mod.hpp"

int main(){
    try{
        (void)tetration_mod(2, 3, 0);
        return 1;
    }catch(const std::invalid_argument&){
    }
    int query_count;
    if(!(std::cin >> query_count)) return 1;
    while(query_count-- > 0){
        std::uint64_t base, height;
        std::uint32_t modulus;
        std::cin >> base >> height >> modulus;
        std::cout << tetration_mod(base, height, modulus) << '\n';
    }
}
