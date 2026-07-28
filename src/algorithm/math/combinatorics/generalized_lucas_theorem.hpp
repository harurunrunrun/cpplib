#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_GENERALIZED_LUCAS_THEOREM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_GENERALIZED_LUCAS_THEOREM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../number_theory/prime/prime_factorization.hpp"

namespace math{
namespace generalized_lucas_internal{

inline u64 inverse_coprime(u64 value, u64 modulus){
    __int128 old_r = static_cast<__int128>(value);
    __int128 r = static_cast<__int128>(modulus);
    __int128 old_s = 1;
    __int128 s = 0;
    while(r != 0){
        const __int128 quotient = old_r / r;
        const __int128 next_r = old_r - quotient * r;
        old_r = r;
        r = next_r;
        const __int128 next_s = old_s - quotient * s;
        old_s = s;
        s = next_s;
    }
    if(old_r != 1)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: inverse does not exist "
            "(generalized_lucas_theorem)."
        );
    }
    old_s %= static_cast<__int128>(modulus);
    if(old_s < 0) old_s += static_cast<__int128>(modulus);
    return static_cast<u64>(old_s);
}

inline u64 checked_prime_power(u64 prime, int exponent){
    if(!is_prime_miller_rabin(prime) || exponent <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid prime power "
            "(generalized_lucas_theorem)."
        );
    }
    u64 result = 1;
    for(int index = 0; index < exponent; ++index){
        if(result > std::numeric_limits<u64>::max() / prime)[[unlikely]]{
            throw std::overflow_error(
                "prime power overflows unsigned long long "
                "(generalized_lucas_theorem)"
            );
        }
        result *= prime;
    }
    return result;
}

inline u64 exponent_in_factorial(u64 value, u64 prime){
    u64 result = 0;
    while(value != 0){
        value /= prime;
        result += value;
    }
    return result;
}

inline u64 multiplicative(
    u64 n,
    u64 k,
    u64 prime,
    int exponent,
    u64 modulus
){
    k = std::min(k, n - k);
    u64 numerator_product = 1 % modulus;
    u64 denominator_product = 1 % modulus;
    long long prime_exponent = 0;
    for(u64 index = 1; index <= k; ++index){
        u64 numerator = n - k + index;
        u64 denominator = index;
        while(numerator % prime == 0){
            numerator /= prime;
            ++prime_exponent;
        }
        while(denominator % prime == 0){
            denominator /= prime;
            --prime_exponent;
        }
        numerator_product = mul_mod_u64(
            numerator_product, numerator % modulus, modulus
        );
        denominator_product = mul_mod_u64(
            denominator_product, denominator % modulus,
            modulus
        );
        if(index == k) break;
    }
    const u64 result = mul_mod_u64(
        numerator_product, inverse_coprime(denominator_product, modulus),
        modulus
    );
    if(prime_exponent >= exponent) return 0;
    if(prime_exponent < 0)[[unlikely]]{
        throw std::logic_error(
            "negative p-adic valuation (generalized_lucas_theorem)"
        );
    }
    return mul_mod_u64(
        result,
        pow_mod_u64(prime, static_cast<u64>(prime_exponent), modulus),
        modulus
    );
}

inline u64 table_method(
    u64 n,
    u64 k,
    u64 prime,
    int exponent,
    u64 modulus
){
    if(modulus > static_cast<u64>(
        std::vector<u64>().max_size() - 1
    ))[[unlikely]]{
        throw std::length_error(
            "prime-power table is too large (generalized_lucas_theorem)"
        );
    }
    std::vector<u64> prefix(static_cast<std::size_t>(modulus) + 1, 1);
    for(u64 value = 1; value <= modulus; ++value){
        prefix[static_cast<std::size_t>(value)] =
            prefix[static_cast<std::size_t>(value - 1)];
        if(value % prime != 0){
            prefix[static_cast<std::size_t>(value)] = mul_mod_u64(
                prefix[static_cast<std::size_t>(value)],
                value,
                modulus
            );
        }
    }
    auto factorial_without_prime = [&](u64 value){
        u64 result = 1 % modulus;
        while(value != 0){
            result = mul_mod_u64(
                result,
                pow_mod_u64(
                    prefix[static_cast<std::size_t>(modulus)],
                    value / modulus,
                    modulus
                ),
                modulus
            );
            result = mul_mod_u64(
                result,
                prefix[static_cast<std::size_t>(value % modulus)],
                modulus
            );
            value /= prime;
        }
        return result;
    };
    const u64 valuation = exponent_in_factorial(n, prime)
        - exponent_in_factorial(k, prime)
        - exponent_in_factorial(n - k, prime);
    if(valuation >= static_cast<u64>(exponent)) return 0;
    u64 result = factorial_without_prime(n);
    result = mul_mod_u64(
        result,
        inverse_coprime(factorial_without_prime(k), modulus),
        modulus
    );
    result = mul_mod_u64(
        result,
        inverse_coprime(factorial_without_prime(n - k), modulus),
        modulus
    );
    return mul_mod_u64(
        result, pow_mod_u64(prime, valuation, modulus), modulus
    );
}

} // namespace generalized_lucas_internal

inline u64 generalized_lucas_binomial_mod_prime_power(
    u64 n,
    u64 k,
    u64 prime,
    int exponent
){
    const u64 modulus =
        generalized_lucas_internal::checked_prime_power(prime, exponent);
    if(k > n) return 0;
    const u64 reduced_k = std::min(k, n - k);
    if(reduced_k < modulus){
        return generalized_lucas_internal::multiplicative(
            n, reduced_k, prime, exponent, modulus
        );
    }
    return generalized_lucas_internal::table_method(
        n, reduced_k, prime, exponent, modulus
    );
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_GENERALIZED_LUCAS_THEOREM_HPP_INCLUDED
