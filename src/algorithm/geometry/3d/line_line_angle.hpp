#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_ANGLE_HPP_INCLUDED

#include <algorithm>

#include "angle.hpp"
#include "base.hpp"
#include "line3_direction.hpp"

inline long double line_line_angle(const Line3& first, const Line3& second){
    const long double directed = angle(
        line3_direction(first), line3_direction(second)
    );
    return std::min(directed, GEOMETRY3D_PI - directed);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_ANGLE_HPP_INCLUDED
