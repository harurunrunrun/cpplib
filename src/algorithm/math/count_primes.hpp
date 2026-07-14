#pragma once

#include <concepts>
#include <cstddef>
#include <ranges>

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

} // namespace math
