#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_AREA_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../point/abs.hpp"
#include "../../point/point_point/cross.hpp"

inline long double area(const Triangle3& triangle){
    return abs(cross(triangle.b - triangle.a, triangle.c - triangle.a)) / 2;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_AREA_HPP_INCLUDED
