#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_ON_CONVEX_HULL_QUERY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_ON_CONVEX_HULL_QUERY_HPP_INCLUDED

#include "convex_polygon_query.hpp"

inline bool on_convex_hull(
    const ConvexPolygonQuery& polygon,
    const Point& point
){
    return polygon.contains(point) == 1;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_ON_CONVEX_HULL_QUERY_HPP_INCLUDED
