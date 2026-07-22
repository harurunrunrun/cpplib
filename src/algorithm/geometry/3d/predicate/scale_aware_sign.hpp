#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SCALE_AWARE_SIGN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SCALE_AWARE_SIGN_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "is_finite.hpp"

inline int scale_aware_sign(
    long double value,
    long double scale,
    long double relative_tolerance = GEOMETRY3D_EPS,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(value) || !geometry3d_is_finite(scale)
        || !geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || scale < 0 || relative_tolerance < 0 || absolute_tolerance < 0)[[unlikely]]{
        throw std::invalid_argument("non-finite value or invalid 3D sign tolerance");
    }
    const long double relative_bound = relative_tolerance * scale;
    if(!geometry3d_is_finite(relative_bound))[[unlikely]]{
        throw std::overflow_error("3D sign tolerance overflow");
    }
    const long double bound = std::max(absolute_tolerance, relative_bound);
    if(value > bound) return 1;
    if(value < -bound) return -1;
    return 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SCALE_AWARE_SIGN_HPP_INCLUDED
