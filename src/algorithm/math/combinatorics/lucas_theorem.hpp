#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LUCAS_THEOREM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LUCAS_THEOREM_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>

#include "../number_theory/modular/modular_arithmetic.hpp"

namespace math{

inline u64 lucas_binomial_mod_prime(u64 n, u64 k, u64 prime){
    if(!is_prime_miller_rabin(prime))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: non-prime modulus "
            "(lucas_binomial_mod_prime)."
        );
    }
    if(k > n) return 0;

    u64 result = 1;
    while(n != 0 || k != 0){
        const u64 n_digit = n % prime;
        const u64 k_digit = k % prime;
        if(k_digit > n_digit) return 0;
        const u64 count = std::min(k_digit, n_digit - k_digit);
        u64 numerator = 1;
        u64 denominator = 1;
        for(u64 index = 1; index <= count; ++index){
            numerator = mul_mod_u64(
                numerator, n_digit - count + index, prime
            );
            denominator = mul_mod_u64(denominator, index, prime);
        }
        const u64 digit_binomial = mul_mod_u64(
            numerator, pow_mod_u64(denominator, prime - 2, prime), prime
        );
        result = mul_mod_u64(result, digit_binomial, prime);
        n /= prime;
        k /= prime;
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_LUCAS_THEOREM_HPP_INCLUDED
