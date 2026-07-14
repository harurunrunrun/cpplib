#pragma once

#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <utility>

#include "prime_factorization.hpp"

namespace math{

namespace pisano_period_internal{

inline std::pair<std::uint64_t, std::uint64_t> fibonacci_pair(
    std::uint64_t index,
    std::uint64_t modulus
){
    if(index == 0) return {0, 1 % modulus};
    const auto [a, b] = fibonacci_pair(index >> 1, modulus);
    const std::uint64_t two_b_minus_a = (2 * b % modulus + modulus - a) % modulus;
    const std::uint64_t c = mul_mod_u64(a, two_b_minus_a, modulus);
    const std::uint64_t d =
        (mul_mod_u64(a, a, modulus) + mul_mod_u64(b, b, modulus)) % modulus;
    if(index & 1) return {d, (c + d) % modulus};
    return {c, d};
}

inline bool is_period(std::uint64_t candidate, std::uint64_t modulus){
    const auto [current, next] = fibonacci_pair(candidate, modulus);
    return current == 0 && next == 1 % modulus;
}

inline std::uint64_t minimal_period_from_multiple(
    std::uint64_t multiple,
    std::uint64_t modulus
){
    std::uint64_t result = multiple;
    for(const auto [prime, exponent]: factorize_pollard_rho(multiple)){
        (void)exponent;
        while(result % prime == 0 && is_period(result / prime, modulus)){
            result /= prime;
        }
    }
    return result;
}

inline std::uint64_t prime_power_period(
    std::uint64_t prime,
    int exponent
){
    std::uint64_t modulus = 1;
    for(int i = 0; i < exponent; ++i) modulus *= prime;
    std::uint64_t candidate;
    if(prime == 2){
        candidate = 3;
    }else if(prime == 5){
        candidate = 20;
    }else{
        const bool five_is_quadratic_residue =
            pow_mod_u64(5 % prime, (prime - 1) / 2, prime) == 1;
        candidate = five_is_quadratic_residue ? prime - 1 : 2 * (prime + 1);
    }
    for(int i = 1; i < exponent; ++i) candidate *= prime;
    return minimal_period_from_multiple(candidate, modulus);
}

} // namespace pisano_period_internal

inline std::uint64_t pisano_period(std::uint64_t modulus){
    if(modulus < 2)[[unlikely]]{
        throw std::invalid_argument("pisano_period requires modulus >= 2");
    }
    std::uint64_t result = 1;
    for(const auto [prime, exponent]: factorize_pollard_rho(modulus)){
        const std::uint64_t component =
            pisano_period_internal::prime_power_period(prime, exponent);
        const std::uint64_t divisor = std::gcd(result, component);
        result = result / divisor * component;
    }
    return result;
}

} // namespace math
