#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_LINE3_DIRECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_LINE3_DIRECTION_HPP_INCLUDED

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

inline Point3 line3_direction(const Line3& line){
    const Point3 direction = line.b - line.a;
    if(geometry3d_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D line");
    }
    return direction;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_LINE3_DIRECTION_HPP_INCLUDED
