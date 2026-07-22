#ifndef CPPLIB_SRC_ALGORITHM_MATH_COUNT_PRIME_ALTERNATING_DIGIT_SUM_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COUNT_PRIME_ALTERNATING_DIGIT_SUM_NUMBERS_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>

namespace math{

namespace prime_alternating_digit_sum_internal{

inline bool is_prime(int value){
    if(value < 2) return false;
    for(int divisor = 2; divisor * divisor <= value; ++divisor){
        if(value % divisor == 0) return false;
    }
    return true;
}

inline std::uint64_t count_up_to(std::uint64_t limit){
    const std::string digits = std::to_string(limit);
    constexpr int DIFFERENCE_OFFSET = 90;
    std::array<std::array<std::uint64_t, 181>, 20> memo{};
    std::array<std::array<bool, 181>, 20> seen{};

    std::function<std::uint64_t(std::size_t, int, bool)> search =
        [&](std::size_t position, int difference, bool tight)
            -> std::uint64_t{
            if(position == digits.size()){
                return is_prime(difference) ? 1 : 0;
            }
            const std::size_t difference_index =
                static_cast<std::size_t>(difference + DIFFERENCE_OFFSET);
            if(!tight && seen[position][difference_index]){
                return memo[position][difference_index];
            }

            const int maximum_digit =
                tight ? digits[position] - '0' : 9;
            const bool even_position_from_right =
                (digits.size() - position) % 2 == 0;
            std::uint64_t result = 0;
            for(int digit = 0; digit <= maximum_digit; ++digit){
                result += search(
                    position + 1,
                    difference +
                        (even_position_from_right ? digit : -digit),
                    tight && digit == maximum_digit
                );
            }
            if(!tight){
                seen[position][difference_index] = true;
                memo[position][difference_index] = result;
            }
            return result;
        };
    return search(0, 0, true);
}

} // namespace prime_alternating_digit_sum_internal

inline std::uint64_t count_prime_alternating_digit_sum_numbers(
    std::uint64_t lower,
    std::uint64_t upper
){
    if(lower > upper)[[unlikely]]{
        throw std::invalid_argument(
            "count_prime_alternating_digit_sum_numbers requires lower <= upper"
        );
    }
    const std::uint64_t before_lower = lower == 0 ? 0 :
        prime_alternating_digit_sum_internal::count_up_to(lower - 1);
    return prime_alternating_digit_sum_internal::count_up_to(upper) -
        before_lower;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COUNT_PRIME_ALTERNATING_DIGIT_SUM_NUMBERS_HPP_INCLUDED
