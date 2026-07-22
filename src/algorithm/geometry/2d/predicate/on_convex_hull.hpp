#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_ON_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_ON_CONVEX_HULL_HPP_INCLUDED

#include <vector>

#include "../query/convex_polygon_query.hpp"

inline bool on_convex_hull(
    const ConvexPolygonQuery& polygon,
    const Point& point
){
    return polygon.contains(point) == 1;
}

inline bool on_convex_hull(
    const std::vector<Point>& polygon,
    const Point& point
){
    return on_convex_hull(ConvexPolygonQuery(polygon), point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_ON_CONVEX_HULL_HPP_INCLUDED
