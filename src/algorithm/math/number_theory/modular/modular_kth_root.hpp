#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_MODULAR_KTH_ROOT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_MODULAR_KTH_ROOT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "modular_arithmetic.hpp"

namespace math{
namespace modular_kth_root_internal{
inline u64 inverse_coprime(u64 value, u64 modulus){
    if(modulus == 1) return 0;
    __int128 old_remainder = static_cast<__int128>(value);
    __int128 remainder = static_cast<__int128>(modulus);
    __int128 old_coefficient = 1;
    __int128 coefficient = 0;
    while(remainder != 0){
        const __int128 quotient = old_remainder / remainder;
        const __int128 next_remainder = old_remainder - quotient * remainder;
        old_remainder = remainder;
        remainder = next_remainder;
        const __int128 next_coefficient = old_coefficient - quotient * coefficient;
        old_coefficient = coefficient;
        coefficient = next_coefficient;
    }
    if(old_remainder != 1)[[unlikely]]{
        throw std::logic_error(
            "non-coprime inverse (modular_kth_roots_prime)"
        );
    }
    old_coefficient %= static_cast<__int128>(modulus);
    if(old_coefficient < 0) old_coefficient += modulus;
    return static_cast<u64>(old_coefficient);
}


inline std::optional<u64> discrete_log_prime(
    u64 generator,
    u64 value,
    u64 prime
){
    if(value == 1) return u64(0);
    u64 block = static_cast<u64>(
        std::sqrt(static_cast<long double>(prime - 1))
    );
    while(static_cast<u128>(block) * block < prime - 1) ++block;
    while(block != 0
        && static_cast<u128>(block - 1) * (block - 1) >= prime - 1){
        --block;
    }
    std::unordered_map<u64, u64> baby;
    if(block <= static_cast<u64>(baby.max_size() / 2)){
        baby.reserve(static_cast<std::size_t>(block * 2));
    }
    u64 current = 1;
    for(u64 exponent = 0; exponent < block; ++exponent){
        baby.emplace(current, exponent);
        current = mul_mod_u64(current, generator, prime);
    }
    const u64 inverse_step =
        pow_mod_u64(pow_mod_u64(generator, block, prime), prime - 2, prime);
    current = value;
    for(u64 giant = 0; giant <= block; ++giant){
        auto iterator = baby.find(current);
        if(iterator != baby.end()){
            const u128 exponent =
                static_cast<u128>(giant) * block + iterator->second;
            if(exponent < prime - 1){
                return static_cast<u64>(exponent);
            }
        }
        current = mul_mod_u64(current, inverse_step, prime);
    }
    return std::nullopt;
}

} // namespace modular_kth_root_internal

inline std::vector<u64> modular_kth_roots_prime(
    u64 value,
    u64 exponent,
    u64 prime
){
    if(!is_prime_miller_rabin(prime) || exponent == 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid argument "
            "(modular_kth_roots_prime)."
        );
    }
    value %= prime;
    if(value == 0) return {0};
    if(prime == 2) return {1};
    const u64 generator = primitive_root(prime);
    const auto logarithm =
        modular_kth_root_internal::discrete_log_prime(
            generator, value, prime
        );
    if(!logarithm) return {};
    const u64 group_order = prime - 1;
    const u64 divisor = std::gcd(exponent, group_order);
    if(*logarithm % divisor != 0) return {};
    const u64 reduced_modulus = group_order / divisor;
    const u64 reduced_exponent = exponent / divisor % reduced_modulus;
    const u64 reduced_logarithm = *logarithm / divisor;
    const u64 inverse = modular_kth_root_internal::inverse_coprime(
        reduced_exponent, reduced_modulus
    );
    const u64 first_exponent =
        mul_mod_u64(reduced_logarithm % reduced_modulus,
            inverse, reduced_modulus);
    std::vector<u64> roots;
    if(divisor > static_cast<u64>(roots.max_size()))[[unlikely]]{
        throw std::length_error(
            "too many modular roots (modular_kth_roots_prime)"
        );
    }
    roots.reserve(static_cast<std::size_t>(divisor));
    for(u64 index = 0; index < divisor; ++index){
        const u64 root_exponent =
            first_exponent + index * reduced_modulus;
        roots.push_back(pow_mod_u64(generator, root_exponent, prime));
    }
    std::sort(roots.begin(), roots.end());
    return roots;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_MODULAR_KTH_ROOT_HPP_INCLUDED
