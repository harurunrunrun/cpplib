#pragma once

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "prime_factorization.hpp"

namespace math{

inline std::vector<std::pair<std::uint64_t, int>>
integer_factorization_15_digits(std::uint64_t value){
    if(value == 0 || value >= 1'000'000'000'000'000ULL)[[unlikely]]{
        throw std::invalid_argument(
            "integer_factorization_15_digits requires 1 <= value < 10^15"
        );
    }
    const auto factors = factorize_pollard_rho(value);
    std::vector<std::pair<std::uint64_t, int>> result;
    result.reserve(factors.size());
    for(const auto [prime, exponent]: factors){
        result.emplace_back(static_cast<std::uint64_t>(prime), exponent);
    }
    return result;
}

} // namespace math
