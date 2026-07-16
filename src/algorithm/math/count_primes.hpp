#pragma once

#include <concepts>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>

#include "prime_factorization.hpp"

namespace math{

template<std::ranges::input_range Range>
requires std::integral<std::ranges::range_value_t<Range>>
std::size_t count_primes(Range&& values){
    using Value = std::ranges::range_value_t<Range>;
    std::size_t result = 0;
    for(const auto value: values){
        if constexpr(std::signed_integral<Value>){
            if(value < 0) continue;
        }
        result += is_prime_miller_rabin(static_cast<u64>(value));
    }
    return result;
}

template<std::ranges::input_range Range>
requires std::integral<std::ranges::range_value_t<Range>>
std::size_t count_primes_with_sieve(
    Range&& values,
    std::size_t maximum_value
){
    if(maximum_value == std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::length_error("prime sieve range is too large");
    }
    std::vector<bool> composite(maximum_value + 1, false);
    for(std::size_t prime = 2;
        prime <= maximum_value / prime;
        ++prime){
        if(composite[prime]) continue;
        std::size_t multiple = prime * prime;
        while(true){
            composite[multiple] = true;
            if(prime > maximum_value - multiple) break;
            multiple += prime;
        }
    }

    using Value = std::ranges::range_value_t<Range>;
    std::size_t result = 0;
    for(const auto value: values){
        if constexpr(std::signed_integral<Value>){
            if(value < 0) continue;
        }
        const std::uintmax_t nonnegative =
            static_cast<std::uintmax_t>(value);
        if(std::cmp_greater(nonnegative, maximum_value))[[unlikely]]{
            throw std::invalid_argument(
                "prime sieve value exceeds maximum_value"
            );
        }
        const std::size_t index = static_cast<std::size_t>(nonnegative);
        result += index >= 2 && !composite[index];
    }
    return result;
}


} // namespace math
