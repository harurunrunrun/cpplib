#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_ROTATE_AROUND_AXIS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_ROTATE_AROUND_AXIS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "cross.hpp"
#include "../scalar/dot.hpp"
#include "unit.hpp"

inline Point3 rotate_around_axis(
    const Point3& point,
    const Point3& axis,
    long double theta
){
    const Point3 direction = unit(axis);
    const long double cosine = std::cos(theta);
    const long double sine = std::sin(theta);
    return point * cosine + cross(direction, point) * sine +
        direction * dot(direction, point) * (1 - cosine);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_ROTATE_AROUND_AXIS_HPP_INCLUDED
