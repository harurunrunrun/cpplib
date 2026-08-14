#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_MINIMUM_WIDTH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_MINIMUM_WIDTH_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../point_set/point_set/convex_hull.hpp"
#include "convex_polygon_minimum_width_points.hpp"

inline long double point_set_minimum_width(std::vector<Point> points){
    return convex_polygon_minimum_width(convex_hull(std::move(points)));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_MINIMUM_WIDTH_HPP_INCLUDED
