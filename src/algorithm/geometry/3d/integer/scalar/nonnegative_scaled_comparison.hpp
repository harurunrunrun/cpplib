#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INTEGER_SCALAR_NONNEGATIVE_SCALED_COMPARISON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INTEGER_SCALAR_NONNEGATIVE_SCALED_COMPARISON_HPP_INCLUDED

#include <cmath>
#include <stdexcept>
#include <utility>

inline int geometry3d_compare_nonnegative_scaled(
    long double left_value,
    long double left_scale,
    long double right_value,
    long double right_scale
){
    if(!std::isfinite(left_value) || !std::isfinite(left_scale) ||
        !std::isfinite(right_value) || !std::isfinite(right_scale) ||
        left_value < 0.0L || right_value < 0.0L ||
        left_scale <= 0.0L || right_scale <= 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "scaled comparison requires finite nonnegative values and positive scales"
        );
    }
    if(left_value == 0.0L || right_value == 0.0L){
        return (left_value > 0.0L) - (right_value > 0.0L);
    }
    const auto representation = [](long double value, long double scale){
        int value_exponent = 0;
        int scale_exponent = 0;
        long double mantissa = std::frexp(value, &value_exponent) *
            std::frexp(scale, &scale_exponent);
        int exponent = value_exponent + scale_exponent;
        if(mantissa < 0.5L){
            mantissa *= 2.0L;
            --exponent;
        }
        return std::pair<long double, int>{mantissa, exponent};
    };
    const auto left = representation(left_value, left_scale);
    const auto right = representation(right_value, right_scale);
    if(left.second != right.second){
        return left.second < right.second ? -1 : 1;
    }
    return (left.first > right.first) - (left.first < right.first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INTEGER_SCALAR_NONNEGATIVE_SCALED_COMPARISON_HPP_INCLUDED
