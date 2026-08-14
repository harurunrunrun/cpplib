#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_MINIMUM_ENCLOSING_RECTANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_MINIMUM_ENCLOSING_RECTANGLE_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../point_set/point_set/convex_hull.hpp"
#include "minimum_enclosing_rectangle_points.hpp"

inline MinimumEnclosingRectangleResult point_set_minimum_enclosing_rectangle(
    std::vector<Point> points
){
    return minimum_enclosing_rectangle(convex_hull(std::move(points)));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_MINIMUM_ENCLOSING_RECTANGLE_HPP_INCLUDED
