#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ON_CONVEX_HULL_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ON_CONVEX_HULL_POINTS_HPP_INCLUDED

#include <vector>

#include "on_convex_hull_query.hpp"

inline bool on_convex_hull(
    const std::vector<Point>& polygon,
    const Point& point
){
    return on_convex_hull(ConvexPolygonQuery(polygon), point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ON_CONVEX_HULL_POINTS_HPP_INCLUDED
