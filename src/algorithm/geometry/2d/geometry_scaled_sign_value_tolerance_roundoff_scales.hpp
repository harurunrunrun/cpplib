#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_GEOMETRY_SCALED_SIGN_VALUE_TOLERANCE_ROUNDOFF_SCALES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_GEOMETRY_SCALED_SIGN_VALUE_TOLERANCE_ROUNDOFF_SCALES_HPP_INCLUDED

#include <cmath>
#include <limits>

#include "constants.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_GEOMETRY_SCALED_SIGN_VALUE_TOLERANCE_ROUNDOFF_SCALES_HPP_INCLUDED
