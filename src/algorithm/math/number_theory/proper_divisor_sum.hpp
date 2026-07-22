#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PROPER_DIVISOR_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PROPER_DIVISOR_SUM_HPP_INCLUDED

#include <cstdint>
#include <limits>
#include <stdexcept>

#include "prime_factorization.hpp"

namespace math{

inline std::uint64_t proper_divisor_sum(std::uint64_t n){
    if(n == 0)[[unlikely]]{
        throw std::invalid_argument("proper_divisor_sum requires n >= 1");
    }
    unsigned __int128 sigma = 1;
    for(const auto [prime, exponent]: factorize_pollard_rho(n)){
        unsigned __int128 term = 1;
        unsigned __int128 power = 1;
        for(int i = 0; i < exponent; ++i){
            power *= prime;
            term += power;
        }
        sigma *= term;
    }
    const unsigned __int128 answer = sigma - n;
    if(answer > std::numeric_limits<std::uint64_t>::max())[[unlikely]]{
        throw std::overflow_error("proper divisor sum does not fit uint64_t");
    }
    return static_cast<std::uint64_t>(answer);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_PROPER_DIVISOR_SUM_HPP_INCLUDED
