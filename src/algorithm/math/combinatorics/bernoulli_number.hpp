#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BERNOULLI_NUMBER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BERNOULLI_NUMBER_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include "../../fft/formal_power_series.hpp"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

template<int MOD>
std::vector<Modint<MOD>> bernoulli_numbers(std::size_t maximum_index){
    static_assert(MOD >= 2);
    if(maximum_index >= static_cast<std::size_t>(MOD - 1))[[unlikely]]{
        throw std::length_error(
            "bernoulli_numbers requires maximum_index + 1 < MOD"
        );
    }
    if(maximum_index >=
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error("bernoulli_numbers target size is too large");
    }

    const std::size_t size = maximum_index + 1;
    std::vector<Modint<MOD>> factorial(size + 1);
    std::vector<Modint<MOD>> inverse_factorial(size + 1);
    factorial[0] = Modint<MOD>(1);
    for(std::size_t index = 1; index <= size; ++index){
        factorial[index] = factorial[index - 1] *
            Modint<MOD>(static_cast<long long>(index));
    }
    inverse_factorial[size] = factorial[size].inv();
    for(std::size_t index = size; index > 0; --index){
        inverse_factorial[index - 1] = inverse_factorial[index] *
            Modint<MOD>(static_cast<long long>(index));
    }

    std::vector<Modint<MOD>> denominator(size);
    for(std::size_t index = 0; index < size; ++index){
        denominator[index] = inverse_factorial[index + 1];
    }
    std::vector<Modint<MOD>> exponential_generating_function =
        fps_inv<MOD>(denominator, size);
    for(std::size_t index = 0; index < size; ++index){
        exponential_generating_function[index] *= factorial[index];
    }
    return exponential_generating_function;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BERNOULLI_NUMBER_HPP_INCLUDED
