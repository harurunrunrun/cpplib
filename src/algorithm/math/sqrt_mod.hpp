#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>

namespace math{
namespace sqrt_mod_internal{

using u64 = std::uint64_t;
using u128 = __uint128_t;

inline u64 multiply_mod(u64 left, u64 right, u64 modulus){
    return static_cast<u64>(static_cast<u128>(left) * right % modulus);
}

inline u64 power_mod(u64 base, u64 exponent, u64 modulus){
    u64 result = 1 % modulus;
    while(exponent != 0){
        if((exponent & 1U) != 0) result = multiply_mod(result, base, modulus);
        base = multiply_mod(base, base, modulus);
        exponent >>= 1U;
    }
    return result;
}

} // namespace sqrt_mod_internal

// Tonelli--Shanks for a prime modulus.  The smaller of the two roots is
// returned to make the result deterministic.
inline std::optional<std::uint64_t> sqrt_mod(
    std::uint64_t value,
    std::uint64_t prime
){
    using namespace sqrt_mod_internal;
    if(prime < 2 || value >= prime)[[unlikely]]{
        throw std::invalid_argument("library assertion fault: invalid argument (sqrt_mod). ");
    }
    if(value == 0) return u64(0);
    if(prime == 2) return value;
    if(power_mod(value, (prime - 1) / 2, prime) != 1) return std::nullopt;

    if(prime % 4 == 3){
        const u64 root = power_mod(value, (prime + 1) / 4, prime);
        return root < prime - root ? root : prime - root;
    }

    u64 odd_part = prime - 1;
    unsigned int exponent_of_two = 0;
    while((odd_part & 1U) == 0){
        odd_part >>= 1U;
        ++exponent_of_two;
    }

    u64 non_residue = 2;
    while(power_mod(non_residue, (prime - 1) / 2, prime) != prime - 1){
        ++non_residue;
    }

    u64 root = power_mod(value, (odd_part + 1) / 2, prime);
    u64 remainder = power_mod(value, odd_part, prime);
    u64 base = power_mod(non_residue, odd_part, prime);
    unsigned int order = exponent_of_two;

    while(remainder != 1){
        unsigned int index = 0;
        u64 squared = remainder;
        while(squared != 1 && index < order){
            squared = multiply_mod(squared, squared, prime);
            ++index;
        }
        if(index == order) return std::nullopt;

        u64 multiplier = base;
        for(unsigned int squaring = 0; squaring + index + 1 < order; ++squaring){
            multiplier = multiply_mod(multiplier, multiplier, prime);
        }
        root = multiply_mod(root, multiplier, prime);
        const u64 multiplier_squared = multiply_mod(multiplier, multiplier, prime);
        remainder = multiply_mod(remainder, multiplier_squared, prime);
        base = multiplier_squared;
        order = index;
    }

    return root < prime - root ? root : prime - root;
}

} // namespace math
