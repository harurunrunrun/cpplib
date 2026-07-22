#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_EULER_TOTIENT_VALUE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_EULER_TOTIENT_VALUE_HPP_INCLUDED

#include <cstdint>

#include "prime_factorization.hpp"

namespace math{

inline std::uint64_t euler_totient_value(std::uint64_t n){
    return euler_phi(n);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_EULER_TOTIENT_VALUE_HPP_INCLUDED
