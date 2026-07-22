#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_CROSS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_CROSS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"

inline Point3 cross(const Point3& left, const Point3& right){
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x,
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_CROSS_HPP_INCLUDED
