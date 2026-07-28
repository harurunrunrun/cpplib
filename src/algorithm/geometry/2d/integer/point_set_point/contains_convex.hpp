#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_POINT_SET_POINT_CONTAINS_CONVEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_POINT_SET_POINT_CONTAINS_CONVEX_HPP_INCLUDED

#include <vector>

#include "../../type/definition/convex_polygon_query.hpp"

inline int contains_convex(
    const std::vector<Point>& polygon,
    const Point& point
){
    return ConvexPolygonQuery(polygon).contains(point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_POINT_SET_POINT_CONTAINS_CONVEX_HPP_INCLUDED
