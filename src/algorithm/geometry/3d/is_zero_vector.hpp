#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_IS_ZERO_VECTOR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_IS_ZERO_VECTOR_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"
#include "scale_aware_sign.hpp"

inline bool is_zero_vector(
    const Point3& vector,
    long double reference_scale = 1.0L,
    long double relative_tolerance = GEOMETRY3D_EPS,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(vector))[[unlikely]]{
        throw std::invalid_argument("non-finite 3D vector");
    }
    const long double magnitude = std::max({
        std::abs(vector.x), std::abs(vector.y), std::abs(vector.z)
    });
    return scale_aware_sign(
        magnitude,
        reference_scale,
        relative_tolerance,
        absolute_tolerance
    ) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_IS_ZERO_VECTOR_HPP_INCLUDED
