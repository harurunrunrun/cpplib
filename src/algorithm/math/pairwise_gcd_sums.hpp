#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

// For every requested n, computes sum(gcd(left, right)) over
// 1 <= left < right <= n.
inline std::vector<std::uint64_t> pairwise_gcd_sums(
    const std::vector<int>& upper_bounds
){
    int limit = 0;
    for(const int upper_bound: upper_bounds){
        if(upper_bound < 0)[[unlikely]]{
            throw std::invalid_argument(
                "pairwise_gcd_sums requires non-negative upper bounds"
            );
        }
        limit = std::max(limit, upper_bound);
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
    for(int upper_bound = 1; upper_bound <= limit; ++upper_bound){
        prefix[static_cast<std::size_t>(upper_bound)] +=
            prefix[static_cast<std::size_t>(upper_bound - 1)];
    }

    std::vector<std::uint64_t> answers;
    answers.reserve(upper_bounds.size());
    for(const int upper_bound: upper_bounds){
        answers.push_back(
            prefix[static_cast<std::size_t>(upper_bound)]
        );
    }
    return answers;
}

} // namespace math
