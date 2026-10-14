#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_RESTRICTED_STIRLING_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_RESTRICTED_STIRLING_NUMBERS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../fft/formal_power_series/formal_power_series.hpp"

namespace math{
namespace restricted_stirling_internal{

template<int MOD>
std::vector<Modint<MOD>> column(
    std::size_t maximum_order, std::size_t components,
    std::size_t minimum_size, bool cycles
){
    using mint = Modint<MOD>;
    if(minimum_size == 0){
        throw std::invalid_argument("minimum component size must be positive");
    }
    if(maximum_order >= static_cast<std::size_t>(MOD)){
        throw std::length_error("restricted Stirling numbers require maximum_order < MOD");
    }
    std::vector<mint> result(maximum_order + 1);
    if(components == 0){ result[0] = mint(1); return result; }
    if(components > maximum_order / minimum_size) return result;

    std::vector<mint> factorial(maximum_order + 1, mint(1));
    std::vector<mint> inverse_factorial(maximum_order + 1, mint(1));
    for(std::size_t i = 1; i <= maximum_order; ++i){
        factorial[i] = factorial[i - 1] * mint(i);
    }
    inverse_factorial.back() = factorial.back().inv();
    for(std::size_t i = maximum_order; i > 0; --i){
        inverse_factorial[i - 1] = inverse_factorial[i] * mint(i);
    }
    std::vector<mint> component(maximum_order + 1);
    for(std::size_t i = minimum_size; i <= maximum_order; ++i){
        component[i] = cycles ? factorial[i - 1] * inverse_factorial[i]
                              : inverse_factorial[i];
    }
    result = fps_pow<MOD>(component, static_cast<long long>(components), maximum_order + 1);
    for(std::size_t i = 0; i <= maximum_order; ++i){
        result[i] *= factorial[i] * inverse_factorial[components];
    }
    return result;
}

} // namespace restricted_stirling_internal

// Unsigned permutations with exactly fixed_cycles cycles, every cycle of size >= minimum_cycle_size.
template<int MOD>
std::vector<Modint<MOD>> restricted_stirling_numbers_first_kind_fixed_k(
    std::size_t maximum_order, std::size_t fixed_cycles,
    std::size_t minimum_cycle_size
){
    return restricted_stirling_internal::column<MOD>(
        maximum_order, fixed_cycles, minimum_cycle_size, true);
}

// Set partitions with exactly fixed_blocks blocks, every block of size >= minimum_block_size.
template<int MOD>
std::vector<Modint<MOD>> restricted_stirling_numbers_second_kind_fixed_k(
    std::size_t maximum_order, std::size_t fixed_blocks,
    std::size_t minimum_block_size
){
    return restricted_stirling_internal::column<MOD>(
        maximum_order, fixed_blocks, minimum_block_size, false);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_RESTRICTED_STIRLING_NUMBERS_HPP_INCLUDED
