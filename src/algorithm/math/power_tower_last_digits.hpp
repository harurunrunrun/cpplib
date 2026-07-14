#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace math{

struct PowerTowerLastNine{
    std::uint32_t value = 0;
    bool truncated = false;

    std::string to_string() const{
        if(!truncated) return std::to_string(value);
        std::ostringstream stream;
        stream << "..." << std::setw(9) << std::setfill('0') << value;
        return stream.str();
    }
};

namespace power_tower_internal{

inline std::uint64_t totient(std::uint64_t value){
    std::uint64_t result = value;
    for(std::uint64_t prime = 2; prime <= value / prime; ++prime){
        if(value % prime != 0) continue;
        result = result / prime * (prime - 1);
        do value /= prime; while(value % prime == 0);
    }
    if(value > 1) result = result / value * (value - 1);
    return result;
}

inline bool tower_at_least(
    std::uint64_t base,
    std::uint64_t height,
    std::uint64_t limit
){
    if(limit <= 1) return true;
    if(height == 0) return false;
    if(base == 0){
        const std::uint64_t exact = (height % 2 == 0) ? 1 : 0;
        return exact >= limit;
    }
    if(base == 1) return false;
    if(height == 1) return base >= limit;
    std::uint64_t required_exponent = 0;
    std::uint64_t product = 1;
    while(product < limit){
        ++required_exponent;
        if(product > (limit - 1) / base){
            product = limit;
        }else{
            product *= base;
        }
    }
    return tower_at_least(base, height - 1, required_exponent);
}

inline std::uint64_t power_mod(
    std::uint64_t base,
    std::uint64_t exponent,
    std::uint64_t modulus
){
    std::uint64_t result = 1 % modulus;
    base %= modulus;
    while(exponent != 0){
        if(exponent & 1){
            result = static_cast<std::uint64_t>(
                static_cast<unsigned __int128>(result) * base % modulus
            );
        }
        exponent >>= 1;
        if(exponent != 0){
            base = static_cast<std::uint64_t>(
                static_cast<unsigned __int128>(base) * base % modulus
            );
        }
    }
    return result;
}

struct ModularTower{
    std::uint64_t residue;
    bool at_least_modulus;
};

inline ModularTower tower_mod(
    std::uint64_t base,
    std::uint64_t height,
    std::uint64_t modulus
){
    if(modulus == 1) return {0, true};
    if(height == 0) return {1 % modulus, 1 >= modulus};
    if(height == 1) return {base % modulus, base >= modulus};
    const std::uint64_t phi = totient(modulus);
    const ModularTower exponent = tower_mod(base, height - 1, phi);
    const std::uint64_t lifted_exponent = exponent.residue +
        (exponent.at_least_modulus ? phi : 0);
    return {
        power_mod(base, lifted_exponent, modulus),
        tower_at_least(base, height, modulus)
    };
}

} // namespace power_tower_internal

// Computes base tetrated to height. Height zero is one.
inline PowerTowerLastNine power_tower_last_nine(
    std::uint64_t base,
    std::uint64_t height
){
    constexpr std::uint64_t modulus = 1'000'000'000ULL;
    const auto result = power_tower_internal::tower_mod(base, height, modulus);
    return {
        static_cast<std::uint32_t>(result.residue),
        result.at_least_modulus
    };
}

} // namespace math
