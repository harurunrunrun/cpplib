#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>

namespace math{

namespace lucifer_number_internal{

inline bool is_prime_difference(int value){
    if(value < 2) return false;
    for(int divisor = 2; divisor * divisor <= value; ++divisor){
        if(value % divisor == 0) return false;
    }
    return true;
}

inline std::uint64_t count_up_to(std::uint64_t limit){
    const std::string digits = std::to_string(limit);
    constexpr int offset = 90;
    std::array<std::array<std::uint64_t, 181>, 20> memo{};
    std::array<std::array<bool, 181>, 20> seen{};
    std::function<std::uint64_t(std::size_t, int, bool)> dfs =
        [&](std::size_t position, int difference, bool tight)->std::uint64_t{
            if(position == digits.size()){
                return is_prime_difference(difference) ? 1 : 0;
            }
            const int key = difference + offset;
            if(!tight && seen[position][static_cast<std::size_t>(key)]){
                return memo[position][static_cast<std::size_t>(key)];
            }
            const int maximum = tight ? digits[position] - '0' : 9;
            const bool even_from_right = ((digits.size() - position) % 2 == 0);
            std::uint64_t result = 0;
            for(int digit = 0; digit <= maximum; ++digit){
                result += dfs(
                    position + 1,
                    difference + (even_from_right ? digit : -digit),
                    tight && digit == maximum
                );
            }
            if(!tight){
                seen[position][static_cast<std::size_t>(key)] = true;
                memo[position][static_cast<std::size_t>(key)] = result;
            }
            return result;
        };
    return dfs(0, 0, true);
}

} // namespace lucifer_number_internal

inline std::uint64_t lucifer_number_count(
    std::uint64_t left,
    std::uint64_t right
){
    if(left > right)[[unlikely]]{
        throw std::invalid_argument("lucifer_number_count requires left <= right");
    }
    const std::uint64_t before = left == 0 ? 0 :
        lucifer_number_internal::count_up_to(left - 1);
    return lucifer_number_internal::count_up_to(right) - before;
}

} // namespace math
