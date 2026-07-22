#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_PLANE_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_PLANE_ANGLE_HPP_INCLUDED

#include <algorithm>
#include <cmath>

#include "base.hpp"
#include "dot.hpp"
#include "line3_direction.hpp"
#include "plane3_unit_normal.hpp"
#include "unit.hpp"

inline long double line_plane_angle(const Line3& line, const Plane3& plane){
    const Point3 direction = unit(line3_direction(line));
    const Point3 normal = plane3_unit_normal(plane);
    return std::asin(std::clamp(
        std::abs(dot(direction, normal)), 0.0L, 1.0L
    ));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_PLANE_ANGLE_HPP_INCLUDED
