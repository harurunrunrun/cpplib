#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_GARNER_ALGORITHM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_GARNER_ALGORITHM_HPP_INCLUDED

#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "../prime/prime_factorization.hpp"

namespace math{
namespace garner_internal{

inline u64 inverse_coprime(u64 value, u64 modulus){
    if(modulus == 1) return 0;
    __int128 old_remainder = value;
    __int128 remainder = modulus;
    __int128 old_coefficient = 1;
    __int128 coefficient = 0;
    while(remainder != 0){
        const __int128 quotient = old_remainder / remainder;
        const __int128 next_remainder =
            old_remainder - quotient * remainder;
        old_remainder = remainder;
        remainder = next_remainder;
        const __int128 next_coefficient =
            old_coefficient - quotient * coefficient;
        old_coefficient = coefficient;
        coefficient = next_coefficient;
    }
    if(old_remainder != 1)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: moduli are not coprime "
            "(garner_algorithm)."
        );
    }
    old_coefficient %= static_cast<__int128>(modulus);
    if(old_coefficient < 0) old_coefficient += modulus;
    return static_cast<u64>(old_coefficient);
}

} // namespace garner_internal

inline u64 garner_algorithm(
    const std::vector<u64>& remainders,
    const std::vector<u64>& moduli,
    u64 output_modulus
){
    if(remainders.size() != moduli.size() || output_modulus == 0)
        [[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid argument "
            "(garner_algorithm)."
        );
    }
    const std::size_t count = moduli.size();
    for(std::size_t index = 0; index < count; ++index){
        if(moduli[index] <= 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid modulus "
                "(garner_algorithm)."
            );
        }
        for(std::size_t other = 0; other < index; ++other){
            if(std::gcd(moduli[index], moduli[other]) != 1)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: moduli are not coprime "
                    "(garner_algorithm)."
                );
            }
        }
    }
    std::vector<u64> extended_moduli = moduli;
    extended_moduli.push_back(output_modulus);
    std::vector<u64> coefficient(count + 1, 1);
    std::vector<u64> constant(count + 1, 0);
    for(std::size_t index = 0; index < count; ++index){
        const u64 modulus = extended_moduli[index];
        const u64 residue = remainders[index] % modulus;
        const u64 difference =
            (residue + modulus - constant[index]) % modulus;
        const u64 digit = mul_mod_u64(
            difference,
            garner_internal::inverse_coprime(
                coefficient[index] % modulus, modulus
            ),
            modulus
        );
        for(std::size_t other = index + 1; other <= count; ++other){
            const u64 other_modulus = extended_moduli[other];
            constant[other] = (
                constant[other]
                + mul_mod_u64(coefficient[other], digit, other_modulus)
            ) % other_modulus;
            coefficient[other] = mul_mod_u64(
                coefficient[other], modulus % other_modulus, other_modulus
            );
        }
    }
    return constant[count];
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MODULAR_GARNER_ALGORITHM_HPP_INCLUDED
