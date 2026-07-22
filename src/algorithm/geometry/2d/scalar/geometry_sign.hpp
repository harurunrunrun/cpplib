#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_GEOMETRY_SIGN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_GEOMETRY_SIGN_HPP_INCLUDED

#include <cmath>
#include <limits>

#include "../core/constants.hpp"

inline int geometry_sign(long double value){
    if(value > GEOMETRY_EPS) return 1;
    if(value < -GEOMETRY_EPS) return -1;
    return 0;
}

inline int geometry_scaled_sign(
    long double value,
    long double tolerance_scale,
    long double roundoff_scale
){
    const long double tolerance =
        GEOMETRY_EPS * std::abs(tolerance_scale)
        + 64.0L * std::numeric_limits<long double>::epsilon()
            * std::abs(roundoff_scale);
    return (value > tolerance) - (value < -tolerance);
}

inline int geometry_scaled_sign(long double value, long double scale){
    return geometry_scaled_sign(value, scale, scale);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_GEOMETRY_SIGN_HPP_INCLUDED
