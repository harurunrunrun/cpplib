#pragma once

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace math{

inline std::pair<std::uint64_t, std::uint64_t> fraction_tree_nth(
    std::uint64_t one_based_index
){
    if(one_based_index == 0)[[unlikely]]{
        throw std::invalid_argument("fraction_tree_nth requires index >= 1");
    }
    std::uint64_t highest_bit = 1;
    while(highest_bit <= one_based_index / 2) highest_bit <<= 1;
    std::uint64_t numerator = 1;
    std::uint64_t denominator = 1;
    for(highest_bit >>= 1; highest_bit != 0; highest_bit >>= 1){
        if(one_based_index & highest_bit){
            numerator += denominator;
        }else{
            denominator += numerator;
        }
    }
    return {numerator, denominator};
}

} // namespace math
