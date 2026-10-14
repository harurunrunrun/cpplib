#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_RATIONAL_BOUNDS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_RATIONAL_BOUNDS_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace math{

// Best lower/upper fractions with numerator and denominator at most limit.
// The sentinel endpoints 0/1 and 1/0 represent zero and positive infinity.
inline std::pair<std::pair<std::uint64_t, std::uint64_t>,
                 std::pair<std::uint64_t, std::uint64_t>> rational_bounds(
    std::uint64_t numerator, std::uint64_t denominator,
    std::uint64_t numerator_limit,std::uint64_t denominator_limit
){
    if(numerator == 0 || denominator == 0 || numerator_limit == 0 || denominator_limit == 0){
        throw std::invalid_argument("rational_bounds requires positive arguments");
    }
    using wide = __uint128_t;
    std::uint64_t a = 0, b = 1, c = 1, d = 0;
    auto cap = [numerator_limit,denominator_limit](std::uint64_t x, std::uint64_t y,
                       std::uint64_t u, std::uint64_t v){
        std::uint64_t result = std::max(numerator_limit,denominator_limit);
        if(u != 0) result = std::min(result, (numerator_limit - x) / u);
        if(v != 0) result = std::min(result, (denominator_limit - y) / v);
        return result;
    };
    while(true){
        const wide low_gap = wide(numerator) * b - wide(denominator) * a;
        const wide high_gap = wide(denominator) * c - wide(numerator) * d;
        if(low_gap == 0) return {{a,b},{a,b}};
        if(high_gap == 0) return {{c,d},{c,d}};
        if(low_gap >= high_gap){
            const std::uint64_t bound = cap(a,b,c,d);
            if(bound == 0) break;
            const auto step = static_cast<std::uint64_t>(
                std::min<wide>(bound, low_gap / high_gap));
            a += step * c; b += step * d;
        }else{
            const std::uint64_t bound = cap(c,d,a,b);
            if(bound == 0) break;
            const auto step = static_cast<std::uint64_t>(
                std::min<wide>(bound, high_gap / low_gap));
            c += step * a; d += step * b;
        }
    }
    return {{a,b},{c,d}};
}

inline auto rational_bounds(std::uint64_t numerator,std::uint64_t denominator,std::uint64_t limit){
    return rational_bounds(numerator,denominator,limit,limit);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_RATIONAL_BOUNDS_HPP_INCLUDED
