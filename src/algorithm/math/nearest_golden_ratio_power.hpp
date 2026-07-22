#ifndef CPPLIB_SRC_ALGORITHM_MATH_NEAREST_GOLDEN_RATIO_POWER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NEAREST_GOLDEN_RATIO_POWER_HPP_INCLUDED

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace math{
namespace nearest_golden_ratio_power_internal{

inline std::uint64_t multiply_mod(
    std::uint64_t left,
    std::uint64_t right,
    std::uint64_t modulus
){
    return static_cast<std::uint64_t>(
        static_cast<unsigned __int128>(left) * right % modulus
    );
}

inline std::uint64_t add_mod(
    std::uint64_t left,
    std::uint64_t right,
    std::uint64_t modulus
){
    return left >= modulus - right
        ? left - (modulus - right)
        : left + right;
}

inline std::pair<std::uint64_t, std::uint64_t> fibonacci_pair_mod(
    std::uint64_t index,
    std::uint64_t modulus
){
    if(index == 0) return {0, 1 % modulus};
    const auto [value, next] = fibonacci_pair_mod(index / 2, modulus);
    const std::uint64_t twice_next = next >= modulus - next
        ? next - (modulus - next)
        : next + next;
    const std::uint64_t difference = twice_next >= value
        ? twice_next - value
        : modulus - (value - twice_next);
    const std::uint64_t doubled = multiply_mod(value, difference, modulus);
    const std::uint64_t advanced = add_mod(
        multiply_mod(value, value, modulus),
        multiply_mod(next, next, modulus),
        modulus
    );
    if((index & 1U) == 0) return {doubled, advanced};
    const std::uint64_t following = advanced >= modulus - doubled
        ? advanced - (modulus - doubled)
        : advanced + doubled;
    return {advanced, following};
}

} // namespace nearest_golden_ratio_power_internal

inline std::uint64_t nearest_golden_ratio_power_mod(
    std::uint64_t exponent,
    std::uint64_t modulus = 1'000'000'007ULL
){
    if(modulus == 0)[[unlikely]]{
        throw std::invalid_argument("nearest golden-ratio power requires positive modulus");
    }
    if(exponent == 0) return 1 % modulus;
    if(exponent == 1) return 2 % modulus;
    const auto [fibonacci, next_fibonacci] =
        nearest_golden_ratio_power_internal::fibonacci_pair_mod(
            exponent, modulus
        );
    const std::uint64_t twice_next = next_fibonacci >= modulus - next_fibonacci
        ? next_fibonacci - (modulus - next_fibonacci)
        : next_fibonacci + next_fibonacci;
    return twice_next >= fibonacci
        ? twice_next - fibonacci
        : modulus - (fibonacci - twice_next);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NEAREST_GOLDEN_RATIO_POWER_HPP_INCLUDED
