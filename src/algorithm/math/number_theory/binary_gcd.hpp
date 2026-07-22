#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_BINARY_GCD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_BINARY_GCD_HPP_INCLUDED

#include <bit>
#include <cstdint>
#include <utility>

namespace math{

namespace binary_gcd_internal{

constexpr std::uint64_t magnitude(std::int64_t value) noexcept{
    if(value >= 0) return static_cast<std::uint64_t>(value);
    return static_cast<std::uint64_t>(-(value + 1)) + 1;
}

} // namespace binary_gcd_internal

// Stein's algorithm for the full uint64_t domain.
constexpr std::uint64_t binary_gcd(
    std::uint64_t first,
    std::uint64_t second
) noexcept{
    if(first == 0) return second;
    if(second == 0) return first;

    const int common_shift = std::countr_zero(first | second);
    first >>= std::countr_zero(first);
    do{
        second >>= std::countr_zero(second);
        if(second < first) std::swap(first, second);
        second -= first;
    }while(second != 0);
    return first << common_shift;
}

// The unsigned return type is intentional: gcd(INT64_MIN, 0) is 2^63.
constexpr std::uint64_t binary_gcd_signed(
    std::int64_t first,
    std::int64_t second
) noexcept{
    return binary_gcd(
        binary_gcd_internal::magnitude(first),
        binary_gcd_internal::magnitude(second)
    );
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_BINARY_GCD_HPP_INCLUDED
