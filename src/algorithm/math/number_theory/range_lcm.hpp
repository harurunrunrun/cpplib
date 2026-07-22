#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_RANGE_LCM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_RANGE_LCM_HPP_INCLUDED

#include <concepts>
#include <ranges>

#include "basic_number_theory.hpp"

namespace math{

template<std::ranges::input_range Range>
requires std::integral<std::ranges::range_value_t<Range>>
long long range_lcm(Range&& values){
    long long result = 1;
    for(const auto value: values){
        result = lcm(result, static_cast<long long>(value));
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_RANGE_LCM_HPP_INCLUDED
