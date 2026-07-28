#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_FAREY_SEQUENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_FAREY_SEQUENCE_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

inline std::vector<std::pair<int, int>> farey_sequence(int order){
    if(order <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: nonpositive order (farey_sequence)."
        );
    }
    std::vector<std::pair<int, int>> result;
    int left_numerator = 0;
    int left_denominator = 1;
    int right_numerator = 1;
    int right_denominator = order;
    result.emplace_back(left_numerator, left_denominator);
    while(right_numerator <= right_denominator){
        result.emplace_back(right_numerator, right_denominator);
        const long long quotient =
            (order + left_denominator) / right_denominator;
        const long long next_numerator =
            quotient * right_numerator - left_numerator;
        const long long next_denominator =
            quotient * right_denominator - left_denominator;
        left_numerator = right_numerator;
        left_denominator = right_denominator;
        right_numerator = static_cast<int>(next_numerator);
        right_denominator = static_cast<int>(next_denominator);
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_ARITHMETIC_FAREY_SEQUENCE_HPP_INCLUDED
