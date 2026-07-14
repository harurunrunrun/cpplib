#pragma once

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace math{

namespace fibonacci_range_sum_internal{

inline std::pair<std::uint64_t, std::uint64_t> pair(
    std::uint64_t index,
    std::uint64_t modulus
){
    if(index == 0) return {0, 1 % modulus};
    const auto [a, b] = pair(index >> 1, modulus);
    const std::uint64_t two_b_minus_a = (2 * b % modulus + modulus - a) % modulus;
    const std::uint64_t c = static_cast<std::uint64_t>(
        static_cast<unsigned __int128>(a) * two_b_minus_a % modulus
    );
    const std::uint64_t d = static_cast<std::uint64_t>(
        (static_cast<unsigned __int128>(a) * a +
            static_cast<unsigned __int128>(b) * b) % modulus
    );
    if(index & 1) return {d, (c + d) % modulus};
    return {c, d};
}

} // namespace fibonacci_range_sum_internal

inline std::uint64_t fibonacci_range_sum(
    std::uint64_t left,
    std::uint64_t right,
    std::uint64_t modulus = 1'000'000'007ULL
){
    if(left > right || modulus == 0)[[unlikely]]{
        throw std::invalid_argument("invalid fibonacci_range_sum range or modulus");
    }
    const auto after_right =
        fibonacci_range_sum_internal::pair(right + 2, modulus).first;
    const auto through_left_minus_one =
        fibonacci_range_sum_internal::pair(left + 1, modulus).first;
    return (after_right + modulus - through_left_minus_one) % modulus;
}

} // namespace math
