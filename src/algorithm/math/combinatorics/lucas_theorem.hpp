#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LUCAS_THEOREM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LUCAS_THEOREM_HPP_INCLUDED

#include "generalized_lucas_theorem.hpp"

namespace math{

inline u64 lucas_binomial_mod_prime(u64 n, u64 k, u64 prime){
    return generalized_lucas_binomial_mod_prime_power(n, k, prime, 1);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LUCAS_THEOREM_HPP_INCLUDED
