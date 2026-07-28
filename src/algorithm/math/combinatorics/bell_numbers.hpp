#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BELL_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BELL_NUMBERS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "stirling_numbers.hpp"
#include "../../fft/formal_power_series/formal_power_series.hpp"

namespace math{

template<int MOD>
std::vector<Modint<MOD>> bell_numbers(std::size_t maximum_index){
    std::vector<Modint<MOD>> factorial;
    std::vector<Modint<MOD>> inverse_factorial;
    stirling_internal::factorial_tables<MOD>(
        maximum_index, factorial, inverse_factorial
    );
    std::vector<Modint<MOD>> exponent(maximum_index + 1);
    for(std::size_t index = 1; index <= maximum_index; ++index){
        exponent[index] = inverse_factorial[index];
    }
    std::vector<Modint<MOD>> result =
        fps_exp<MOD>(exponent, maximum_index + 1);
    for(std::size_t index = 0; index <= maximum_index; ++index){
        result[index] *= factorial[index];
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BELL_NUMBERS_HPP_INCLUDED
