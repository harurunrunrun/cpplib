#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CIRCLE_LINE_INTERSECTION_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CIRCLE_LINE_INTERSECTION_COUNT_HPP_INCLUDED

#include "../point_collection/circle_line_cross_points.hpp"

inline int circle_line_intersection_count(
    const Circle& circle,
    const Line& line
){
    return static_cast<int>(circle_line_cross_points(circle, line).size());
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CIRCLE_LINE_INTERSECTION_COUNT_HPP_INCLUDED
