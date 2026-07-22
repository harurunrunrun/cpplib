#ifndef CPPLIB_SRC_ALGORITHM_MATH_PARTITION_FUNCTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_PARTITION_FUNCTION_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include "../fft/formal_power_series.hpp"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

template<int MOD>
std::vector<Modint<MOD>> partition_numbers(std::size_t maximum_sum){
    if(maximum_sum >=
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error("partition_numbers target size is too large");
    }
    const std::size_t size = maximum_sum + 1;
    std::vector<Modint<MOD>> euler_product(size);
    euler_product[0] = Modint<MOD>(1);
    for(std::uint64_t index = 1;; ++index){
        const std::uint64_t first = index * (3 * index - 1) / 2;
        if(first > maximum_sum) break;
        const Modint<MOD> sign =
            (index & 1U) != 0 ? Modint<MOD>(-1) : Modint<MOD>(1);
        euler_product[static_cast<std::size_t>(first)] += sign;
        const std::uint64_t second = index * (3 * index + 1) / 2;
        if(second <= maximum_sum){
            euler_product[static_cast<std::size_t>(second)] += sign;
        }
    }
    return fps_inv<MOD>(euler_product, size);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_PARTITION_FUNCTION_HPP_INCLUDED
