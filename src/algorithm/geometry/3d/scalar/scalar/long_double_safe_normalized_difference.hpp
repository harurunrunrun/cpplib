#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SCALAR_LONG_DOUBLE_SAFE_NORMALIZED_DIFFERENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SCALAR_LONG_DOUBLE_SAFE_NORMALIZED_DIFFERENCE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

inline long double geometry3d_safe_normalized_difference(
    long double left,
    long double right,
    long double positive_scale
){
    if(!std::isfinite(left) || !std::isfinite(right) ||
        !std::isfinite(positive_scale) || positive_scale <= 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "normalized difference requires finite values and a positive scale"
        );
    }
    const long double result =
        std::signbit(left) == std::signbit(right)
        ? (left - right) / positive_scale
        : left / positive_scale - right / positive_scale;
    if(result == 0.0L && left != right)[[unlikely]]{
        throw std::overflow_error(
            "normalized difference is not representable"
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SCALAR_LONG_DOUBLE_SAFE_NORMALIZED_DIFFERENCE_HPP_INCLUDED
