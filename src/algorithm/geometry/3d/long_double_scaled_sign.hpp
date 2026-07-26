#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LONG_DOUBLE_SCALED_SIGN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LONG_DOUBLE_SCALED_SIGN_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "constants.hpp"

inline int geometry3d_scaled_sign(
    long double value,
    long double nonnegative_characteristic_scale
){
    if(!std::isfinite(value) ||
        !std::isfinite(nonnegative_characteristic_scale) ||
        nonnegative_characteristic_scale < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "scaled sign requires finite value and nonnegative scale"
        );
    }
    const long double tolerance =
        GEOMETRY3D_EPS * nonnegative_characteristic_scale;
    if(value > tolerance) return 1;
    if(value < -tolerance) return -1;
    return 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LONG_DOUBLE_SCALED_SIGN_HPP_INCLUDED
