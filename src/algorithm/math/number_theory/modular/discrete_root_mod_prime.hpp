#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_DISCRETE_ROOT_MOD_PRIME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_DISCRETE_ROOT_MOD_PRIME_HPP_INCLUDED

#include <optional>

#include "modular_kth_root.hpp"

namespace math{

inline std::optional<u64> discrete_root_mod_prime(
    u64 value,
    u64 exponent,
    u64 prime
){
    std::vector<u64> roots =
        modular_kth_roots_prime(value, exponent, prime);
    if(roots.empty()) return std::nullopt;
    return roots.front();
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_DISCRETE_ROOT_MOD_PRIME_HPP_INCLUDED
