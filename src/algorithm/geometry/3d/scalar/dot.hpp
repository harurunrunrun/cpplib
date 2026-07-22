#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_DOT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_DOT_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"

inline long double dot(const Point3& left, const Point3& right){
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_DOT_HPP_INCLUDED
