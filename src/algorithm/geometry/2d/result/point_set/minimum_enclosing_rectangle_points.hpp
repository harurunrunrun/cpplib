#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_MINIMUM_ENCLOSING_RECTANGLE_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_MINIMUM_ENCLOSING_RECTANGLE_POINTS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "minimum_enclosing_rectangle_normalized.hpp"

inline MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    std::vector<Point> polygon
){
    return minimum_enclosing_rectangle(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_MINIMUM_ENCLOSING_RECTANGLE_POINTS_HPP_INCLUDED
