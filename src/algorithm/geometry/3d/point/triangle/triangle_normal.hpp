#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_NORMAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_NORMAL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../point_point/cross.hpp"
#include "../point/unit.hpp"

inline Point3 triangle_normal(const Triangle3& triangle){
    return unit(cross(triangle.b - triangle.a, triangle.c - triangle.a));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_NORMAL_HPP_INCLUDED
