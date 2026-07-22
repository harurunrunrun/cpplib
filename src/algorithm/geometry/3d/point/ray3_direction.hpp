#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY3_DIRECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY3_DIRECTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "../scalar/abs.hpp"
#include "../predicate/geometry3d_sign.hpp"

inline Point3 ray3_direction(const Ray3& ray){
    const Point3 direction = ray.through - ray.origin;
    if(geometry3d_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D ray");
    }
    return direction;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY3_DIRECTION_HPP_INCLUDED
