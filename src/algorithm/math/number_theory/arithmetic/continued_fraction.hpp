#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_CONTINUED_FRACTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_CONTINUED_FRACTION_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

inline std::vector<long long> continued_fraction_expansion(
    long long numerator,
    long long denominator
){
    if(denominator == 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: zero denominator "
            "(continued_fraction_expansion)."
        );
    }
    std::vector<long long> result;
    while(denominator != 0){
        long long quotient = numerator / denominator;
        long long remainder = numerator % denominator;
        if(remainder != 0
            && ((remainder < 0) != (denominator < 0))){
            --quotient;
            remainder += denominator;
        }
        result.push_back(quotient);
        numerator = denominator;
        denominator = remainder;
    }
    if(result.size() > 1 && result.back() == 1){
        result.pop_back();
        if(result.back() == std::numeric_limits<long long>::max())
            [[unlikely]]{
            throw std::overflow_error(
                "continued fraction coefficient overflows long long"
            );
        }
        ++result.back();
    }
    return result;
}

inline std::vector<std::pair<__int128, __int128>>
continued_fraction_convergents(const std::vector<long long>& coefficients){
    __int128 previous_numerator = 0;
    __int128 numerator = 1;
    __int128 previous_denominator = 1;
    __int128 denominator = 0;
    std::vector<std::pair<__int128, __int128>> result;
    result.reserve(coefficients.size());
    for(long long coefficient: coefficients){
        const __int128 next_numerator =
            static_cast<__int128>(coefficient) * numerator
            + previous_numerator;
        const __int128 next_denominator =
            static_cast<__int128>(coefficient) * denominator
            + previous_denominator;
        previous_numerator = numerator;
        numerator = next_numerator;
        previous_denominator = denominator;
        denominator = next_denominator;
        result.emplace_back(numerator, denominator);
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_CONTINUED_FRACTION_HPP_INCLUDED
