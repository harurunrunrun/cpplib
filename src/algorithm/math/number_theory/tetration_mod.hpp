#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_TETRATION_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_TETRATION_MOD_HPP_INCLUDED

#include <cstdint>
#include <stdexcept>

namespace tetration_mod_internal{

inline std::uint32_t totient(std::uint32_t value){
    std::uint32_t result = value;
    std::uint32_t remaining = value;
    for(std::uint32_t prime = 2;
        static_cast<std::uint64_t>(prime) * prime <= remaining; ++prime){
        if(remaining % prime != 0) continue;
        result = result / prime * (prime - 1);
        do{
            remaining /= prime;
        }while(remaining % prime == 0);
    }
    if(remaining > 1) result = result / remaining * (remaining - 1);
    return result;
}

inline bool tower_at_least(std::uint64_t base, std::uint64_t height,
                           std::uint64_t limit){
    if(limit == 0) return true;
    if(height == 0) return 1 >= limit;
    if(base == 0){
        const std::uint64_t value = (height % 2 == 0 ? 1 : 0);
        return value >= limit;
    }
    if(base == 1) return 1 >= limit;
    if(limit <= 1) return true;

    std::uint64_t power = 1;
    std::uint64_t required_exponent = 0;
    while(power < limit){
        ++required_exponent;
        if(power > (limit - 1) / base){
            power = limit;
        }else{
            power *= base;
        }
    }
    return tower_at_least(base, height - 1, required_exponent);
}

inline std::uint32_t power(std::uint64_t base, std::uint64_t exponent,
                           std::uint32_t modulus){
    std::uint64_t current = base % modulus;
    std::uint64_t result = 1 % modulus;
    while(exponent > 0){
        if((exponent & 1U) != 0) result = result * current % modulus;
        current = current * current % modulus;
        exponent >>= 1;
    }
    return static_cast<std::uint32_t>(result);
}

inline std::uint32_t solve(std::uint64_t base, std::uint64_t height,
                           std::uint32_t modulus){
    if(modulus == 1) return 0;
    if(height == 0) return 1 % modulus;

    const std::uint32_t phi = totient(modulus);
    std::uint64_t exponent = solve(base, height - 1, phi);
    if(tower_at_least(base, height - 1, phi)) exponent += phi;
    return power(base, exponent, modulus);
}

} // namespace tetration_mod_internal

inline std::uint32_t tetration_mod(std::uint64_t base, std::uint64_t height,
                                   std::uint32_t modulus){
    if(modulus == 0) throw std::invalid_argument("tetration_mod requires modulus > 0");
    return tetration_mod_internal::solve(base, height, modulus);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_TETRATION_MOD_HPP_INCLUDED
