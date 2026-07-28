#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BINOMIAL_MOD_COMPOSITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BINOMIAL_MOD_COMPOSITE_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <vector>

#include "generalized_lucas_theorem.hpp"

namespace math{

inline u64 binomial_mod_composite(u64 n, u64 k, u64 modulus){
    if(modulus == 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: zero modulus "
            "(binomial_mod_composite)."
        );
    }
    if(modulus == 1 || k > n) return 0;
    const auto factors = factorize_pollard_rho(modulus);
    u64 result = 0;
    u64 current_modulus = 1;
    for(auto [prime, exponent]: factors){
        const u64 prime_power =
            generalized_lucas_internal::checked_prime_power(
                prime, exponent
            );
        const u64 residue =
            generalized_lucas_binomial_mod_prime_power(
                n, k, prime, exponent
            );
        const u64 difference =
            (residue + prime_power - result % prime_power) % prime_power;
        const u64 inverse =
            generalized_lucas_internal::inverse_coprime(
                current_modulus % prime_power, prime_power
            );
        const u64 multiplier =
            mul_mod_u64(difference, inverse, prime_power);
        const __uint128_t merged =
            static_cast<__uint128_t>(result)
            + static_cast<__uint128_t>(current_modulus) * multiplier;
        if(current_modulus
            > std::numeric_limits<u64>::max() / prime_power)[[unlikely]]{
            throw std::overflow_error(
                "CRT modulus overflows unsigned long long "
                "(binomial_mod_composite)"
            );
        }
        current_modulus *= prime_power;
        result = static_cast<u64>(merged % current_modulus);
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BINOMIAL_MOD_COMPOSITE_HPP_INCLUDED
