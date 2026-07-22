#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_CONTAINS_CONVEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_CONTAINS_CONVEX_HPP_INCLUDED

#include <vector>

#include "../query/convex_polygon_query.hpp"

inline int contains_convex(
    const std::vector<Point>& polygon,
    const Point& point
){
    return ConvexPolygonQuery(polygon).contains(point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_CONTAINS_CONVEX_HPP_INCLUDED
