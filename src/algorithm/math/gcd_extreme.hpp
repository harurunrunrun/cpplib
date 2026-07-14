#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

// Answers G(n) = sum_{1 <= i < j <= n} gcd(i, j) for a batch of n's.
inline std::vector<std::uint64_t> gcd_extreme(
    const std::vector<int>& queries
){
    int limit = 0;
    for(const int n: queries){
        if(n < 0)[[unlikely]]{
            throw std::invalid_argument("gcd_extreme requires non-negative n");
        }
        limit = std::max(limit, n);
    }

    std::vector<std::uint32_t> phi(static_cast<std::size_t>(limit) + 1);
    for(int i = 0; i <= limit; ++i){
        phi[static_cast<std::size_t>(i)] = static_cast<std::uint32_t>(i);
    }
    for(int p = 2; p <= limit; ++p){
        if(phi[static_cast<std::size_t>(p)] != static_cast<std::uint32_t>(p)){
            continue;
        }
        for(int multiple = p; multiple <= limit; multiple += p){
            auto& value = phi[static_cast<std::size_t>(multiple)];
            value -= value / static_cast<std::uint32_t>(p);
        }
    }

    std::vector<std::uint64_t> prefix(static_cast<std::size_t>(limit) + 1);
    for(int quotient = 2; quotient <= limit; ++quotient){
        const std::uint64_t coefficient = phi[static_cast<std::size_t>(quotient)];
        for(int value = quotient; value <= limit; value += quotient){
            prefix[static_cast<std::size_t>(value)] +=
                coefficient * static_cast<std::uint64_t>(value / quotient);
        }
    }
    for(int n = 1; n <= limit; ++n){
        prefix[static_cast<std::size_t>(n)] +=
            prefix[static_cast<std::size_t>(n - 1)];
    }

    std::vector<std::uint64_t> result;
    result.reserve(queries.size());
    for(const int n: queries){
        result.push_back(prefix[static_cast<std::size_t>(n)]);
    }
    return result;
}

} // namespace math
