#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_DISCRETE_ROOT_MOD_PRIME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_DISCRETE_ROOT_MOD_PRIME_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <unordered_map>

#include "modular_arithmetic.hpp"

namespace math{
namespace discrete_root_detail{

inline u64 ceil_square_root(u64 value){
    u64 lower = 0;
    u64 upper = u64{1} << 32;
    while(lower < upper){
        const u64 middle = lower + (upper - lower) / 2;
        if(static_cast<u128>(middle) * middle >= value){
            upper = middle;
        }else{
            lower = middle + 1;
        }
    }
    return lower;
}

inline std::optional<u64> discrete_log_primitive_root(
    u64 value,
    u64 generator,
    u64 prime
){
    const u64 order = prime - 1;
    const u64 block_size = ceil_square_root(order);
    std::unordered_map<u64, u64> baby_step;
    if(block_size <= baby_step.max_size()){
        baby_step.reserve(static_cast<std::size_t>(block_size));
    }
    u64 power = 1;
    for(u64 index = 0; index < block_size; ++index){
        baby_step.try_emplace(power, index);
        power = mul_mod_u64(power, generator, prime);
    }
    const u64 inverse_block = pow_mod_u64(power, prime - 2, prime);
    u64 giant_step = value;
    for(u64 block = 0; block <= block_size; ++block){
        const auto iterator = baby_step.find(giant_step);
        if(iterator != baby_step.end()){
            return static_cast<u64>(
                (static_cast<u128>(block) * block_size
                    + iterator->second) % order
            );
        }
        giant_step = mul_mod_u64(giant_step, inverse_block, prime);
    }
    return std::nullopt;
}

inline u64 inverse_coprime(u64 value, u64 modulus){
    if(modulus == 1) return 0;
    u64 old_remainder = modulus;
    u64 remainder = value % modulus;
    __int128 old_coefficient = 0;
    __int128 coefficient = 1;
    while(remainder != 0){
        const u64 quotient = old_remainder / remainder;
        const u64 next_remainder =
            old_remainder - quotient * remainder;
        old_remainder = remainder;
        remainder = next_remainder;
        const __int128 next_coefficient =
            old_coefficient
            - static_cast<__int128>(quotient) * coefficient;
        old_coefficient = coefficient;
        coefficient = next_coefficient;
    }
    __int128 normalized =
        old_coefficient % static_cast<__int128>(modulus);
    if(normalized < 0) normalized += modulus;
    return static_cast<u64>(normalized);
}

} // namespace discrete_root_detail

inline std::optional<u64> discrete_root_mod_prime(
    u64 value,
    u64 exponent,
    u64 prime
){
    if(exponent == 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: zero exponent "
            "(discrete_root_mod_prime)."
        );
    }
    if(!is_prime_miller_rabin(prime))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: non-prime modulus "
            "(discrete_root_mod_prime)."
        );
    }
    value %= prime;
    if(value == 0) return 0;
    if(prime == 2) return 1;

    const u64 generator = primitive_root(prime);
    const auto logarithm =
        discrete_root_detail::discrete_log_primitive_root(
            value, generator, prime
        );
    if(!logarithm) return std::nullopt;

    const u64 order = prime - 1;
    const u64 divisor = std::gcd(exponent, order);
    if(*logarithm % divisor != 0) return std::nullopt;
    const u64 reduced_order = order / divisor;
    const u64 first_exponent = reduced_order == 1 ? 0 : mul_mod_u64(
        (*logarithm / divisor) % reduced_order,
        discrete_root_detail::inverse_coprime(
            (exponent / divisor) % reduced_order, reduced_order
        ),
        reduced_order
    );

    u64 answer = prime - 1;
    for(u64 index = 0; index < divisor; ++index){
        const u64 root_exponent = static_cast<u64>(
            static_cast<u128>(index) * reduced_order + first_exponent
        );
        answer = std::min(
            answer, pow_mod_u64(generator, root_exponent, prime)
        );
    }
    return answer;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_DISCRETE_ROOT_MOD_PRIME_HPP_INCLUDED
