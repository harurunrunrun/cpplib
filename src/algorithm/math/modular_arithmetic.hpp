#pragma once

#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "basic_number_theory.hpp"
#include "prime_factorization.hpp"

namespace math{

struct ChineseRemainderResult{
    long long remainder;
    long long modulus;
};

inline std::optional<ChineseRemainderResult> chinese_remainder(
    long long first_remainder,
    long long first_modulus,
    long long second_remainder,
    long long second_modulus
){
    if(first_modulus <= 0 || second_modulus <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid modulus (chinese_remainder)."
        );
    }
    first_remainder %= first_modulus;
    second_remainder %= second_modulus;
    if(first_remainder < 0) first_remainder += first_modulus;
    if(second_remainder < 0) second_remainder += second_modulus;

    const ExtGcdResult extended = ext_gcd(first_modulus, second_modulus);
    const long long difference = second_remainder - first_remainder;
    if(difference % extended.g != 0) return std::nullopt;

    const long long reduced_modulus = second_modulus / extended.g;
    __int128 multiplier = static_cast<__int128>(difference / extended.g) * extended.x;
    multiplier %= reduced_modulus;
    if(multiplier < 0) multiplier += reduced_modulus;

    const __int128 combined_modulus =
        static_cast<__int128>(first_modulus / extended.g) * second_modulus;
    if(combined_modulus > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::runtime_error("library assertion fault: overflow (chinese_remainder).");
    }
    __int128 remainder = first_remainder + static_cast<__int128>(first_modulus) * multiplier;
    remainder %= combined_modulus;
    return ChineseRemainderResult{
        static_cast<long long>(remainder),
        static_cast<long long>(combined_modulus)
    };
}

inline std::optional<ChineseRemainderResult> chinese_remainder(
    const std::vector<long long>& remainders,
    const std::vector<long long>& moduli
){
    if(remainders.size() != moduli.size())[[unlikely]]{
        throw std::runtime_error("library assertion fault: size violation (chinese_remainder).");
    }
    ChineseRemainderResult result{0, 1};
    for(std::size_t i = 0; i < remainders.size(); i++){
        auto merged = chinese_remainder(
            result.remainder,
            result.modulus,
            remainders[i],
            moduli[i]
        );
        if(!merged) return std::nullopt;
        result = *merged;
    }
    return result;
}

inline u64 primitive_root(u64 prime){
    if(!is_prime_miller_rabin(prime))[[unlikely]]{
        throw std::runtime_error("library assertion fault: non-prime modulus (primitive_root).");
    }
    if(prime == 2) return 1;

    const auto factors = factorize_pollard_rho(prime - 1);
    for(u64 candidate = 2; candidate < prime; candidate++){
        bool valid = true;
        for(const auto& [factor, exponent]: factors){
            (void)exponent;
            if(pow_mod_u64(candidate, (prime - 1) / factor, prime) == 1){
                valid = false;
                break;
            }
        }
        if(valid) return candidate;
    }
    throw std::runtime_error("library assertion fault: primitive root does not exist.");
}

} // namespace math
