#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BIT_POPULATION_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BIT_POPULATION_COUNT_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace math{

using WideBitCount = unsigned __int128;

constexpr WideBitCount count_bit_ones_inclusive(
    std::uint64_t maximum,
    unsigned bit
){
    if(bit >= 64)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: bit out of range (count_bit_ones_inclusive)."
        );
    }
    const WideBitCount length = static_cast<WideBitCount>(maximum) + 1;
    const WideBitCount half_cycle = WideBitCount{1} << bit;
    const WideBitCount cycle = half_cycle << 1;
    const WideBitCount complete = length / cycle;
    const WideBitCount remainder = length % cycle;
    return complete * half_cycle
        + (remainder > half_cycle ? remainder - half_cycle : 0);
}

constexpr WideBitCount count_bit_ones_half_open(
    std::uint64_t right,
    unsigned bit
){
    if(bit >= 64)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: bit out of range (count_bit_ones_half_open)."
        );
    }
    if(right == 0) return 0;
    return count_bit_ones_inclusive(right - 1, bit);
}

constexpr std::array<WideBitCount, 64> bit_one_counts_inclusive(
    std::uint64_t maximum
){
    std::array<WideBitCount, 64> result{};
    for(unsigned bit = 0; bit < 64; ++bit){
        result[bit] = count_bit_ones_inclusive(maximum, bit);
    }
    return result;
}

constexpr std::array<WideBitCount, 64> bit_one_counts_half_open(
    std::uint64_t right
){
    std::array<WideBitCount, 64> result{};
    for(unsigned bit = 0; bit < 64; ++bit){
        result[bit] = count_bit_ones_half_open(right, bit);
    }
    return result;
}

constexpr WideBitCount total_set_bits_inclusive(std::uint64_t maximum){
    WideBitCount result = 0;
    for(const WideBitCount count: bit_one_counts_inclusive(maximum)){
        result += count;
    }
    return result;
}

constexpr WideBitCount total_set_bits_half_open(std::uint64_t right){
    WideBitCount result = 0;
    for(const WideBitCount count: bit_one_counts_half_open(right)){
        result += count;
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BIT_POPULATION_COUNT_HPP_INCLUDED
