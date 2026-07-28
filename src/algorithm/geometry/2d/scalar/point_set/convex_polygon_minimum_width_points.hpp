#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_CONVEX_POLYGON_MINIMUM_WIDTH_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_CONVEX_POLYGON_MINIMUM_WIDTH_POINTS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../polygon/convex_polygon_minimum_width_normalized.hpp"

inline long double convex_polygon_minimum_width(std::vector<Point> polygon){
    return convex_polygon_minimum_width(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_CONVEX_POLYGON_MINIMUM_WIDTH_POINTS_HPP_INCLUDED
