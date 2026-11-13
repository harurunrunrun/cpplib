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
#include "../../aggregate/all/is_finite.hpp"

inline long double area(const Triangle3& triangle){
    if(!geometry3d_is_finite(triangle))[[unlikely]]{
        throw std::invalid_argument("triangle area requires finite points");
    }
    const Point3 product =
        cross(triangle.b - triangle.a, triangle.c - triangle.a);
    if(!geometry3d_is_finite(product))[[unlikely]]{
        throw std::overflow_error("3D triangle area is not representable");
    }
    return abs(product) / 2;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_AREA_HPP_INCLUDED
