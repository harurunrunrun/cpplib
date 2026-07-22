#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_FROM_AXIS_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_FROM_AXIS_ANGLE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "quaternion3.hpp"
#include "is_finite.hpp"

inline Quaternion3 quaternion3_from_axis_angle(
    const Point3& axis,
    long double angle
){
    if(!geometry3d_is_finite(axis) || !std::isfinite(angle))[[unlikely]]{
        throw std::invalid_argument("non-finite quaternion axis-angle input");
    }
    const long double scale = std::max({
        std::abs(axis.x), std::abs(axis.y), std::abs(axis.z)
    });
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero quaternion rotation axis");
    }
    const Point3 scaled = axis / scale;
    const long double length = std::hypot(scaled.x, scaled.y, scaled.z);
    const Point3 direction = scaled / length;
    const long double half = angle / 2.0L;
    const long double sine = std::sin(half);
    return Quaternion3{
        std::cos(half), direction.x * sine,
        direction.y * sine, direction.z * sine,
    }.normalized();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_FROM_AXIS_ANGLE_HPP_INCLUDED
