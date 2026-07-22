#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DIVISOR_COUNT_PREFIX_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DIVISOR_COUNT_PREFIX_SUM_HPP_INCLUDED

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

using divisor_count_prefix_sum_value = unsigned __int128;

namespace divisor_count_prefix_sum_detail{

struct Vector{
    std::uint64_t x;
    std::uint64_t y;
};

inline bool translated_above_hyperbola(
    std::uint64_t n,
    std::uint64_t x,
    std::uint64_t y,
    const Vector& direction
){
    if(direction.y >= y) return false;
    return static_cast<divisor_count_prefix_sum_value>(x + direction.x) *
               (y - direction.y) >
           n;
}

inline std::uint64_t integer_square_root(std::uint64_t n){
    auto root = static_cast<std::uint64_t>(std::sqrt(static_cast<long double>(n)));
    while(static_cast<divisor_count_prefix_sum_value>(root + 1) * (root + 1) <= n) ++root;
    while(static_cast<divisor_count_prefix_sum_value>(root) * root > n) --root;
    return root;
}

} // namespace divisor_count_prefix_sum_detail

// Returns sum_{i=1}^n tau(i), where tau(i) is the number of positive divisors.
inline divisor_count_prefix_sum_value divisor_count_prefix_sum(std::uint64_t n){
    if(n == 0) return 0;
    if(n >= (std::uint64_t{1} << 63)){
        throw std::invalid_argument("divisor_count_prefix_sum requires n < 2^63");
    }

    using divisor_count_prefix_sum_detail::Vector;
    using divisor_count_prefix_sum_detail::translated_above_hyperbola;

    const std::uint64_t square_root =
        divisor_count_prefix_sum_detail::integer_square_root(n);
    std::uint64_t x = n / square_root;
    std::uint64_t y = square_root + 1;
    divisor_count_prefix_sum_value half = 0;

    std::vector<Vector> stern_brocot_stack;
    stern_brocot_stack.reserve(256);
    stern_brocot_stack.push_back({1, 0});
    stern_brocot_stack.push_back({1, 1});

    for(;;){
        Vector left = stern_brocot_stack.back();
        stern_brocot_stack.pop_back();
        while(translated_above_hyperbola(n, x, y, left)){
            half += static_cast<divisor_count_prefix_sum_value>(x) * left.y;
            half += static_cast<divisor_count_prefix_sum_value>(left.y + 1) *
                    (left.x - 1) / 2;
            x += left.x;
            y -= left.y;
        }

        if(static_cast<divisor_count_prefix_sum_value>(y) * y * y <= n) break;

        Vector right = stern_brocot_stack.back();
        while(!translated_above_hyperbola(n, x, y, right)){
            left = right;
            stern_brocot_stack.pop_back();
            right = stern_brocot_stack.back();
        }

        for(;;){
            const Vector middle{left.x + right.x, left.y + right.y};
            if(translated_above_hyperbola(n, x, y, middle)){
                right = middle;
                stern_brocot_stack.push_back(right);
                continue;
            }
            const auto next_x = static_cast<divisor_count_prefix_sum_value>(x) + middle.x;
            if(next_x * next_x * right.y >=
               static_cast<divisor_count_prefix_sum_value>(n) * right.x){
                break;
            }
            left = middle;
        }
    }

    for(std::uint64_t i = 1; i < y; ++i) half += n / i;
    return half * 2 - static_cast<divisor_count_prefix_sum_value>(square_root) * square_root;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DIVISOR_COUNT_PREFIX_SUM_HPP_INCLUDED
