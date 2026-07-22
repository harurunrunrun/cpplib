#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_ANGLE_HPP_INCLUDED

#include <algorithm>
#include <cmath>

#include "../core/geometry_primitives.hpp"
#include "dot.hpp"
#include "../point/plane3_unit_normal.hpp"

inline long double plane_plane_angle(
    const Plane3& first,
    const Plane3& second
){
    return std::acos(std::clamp(
        std::abs(dot(
            plane3_unit_normal(first), plane3_unit_normal(second)
        )),
        0.0L,
        1.0L
    ));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_ANGLE_HPP_INCLUDED
