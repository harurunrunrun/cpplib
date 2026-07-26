#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTERSECT_LINE_CONVEX_POLYGON_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTERSECT_LINE_CONVEX_POLYGON_POINTS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "intersect_line_convex_polygon_normalized.hpp"

inline bool intersect_line_convex_polygon(
    const Line& line,
    std::vector<Point> polygon
){
    return intersect_line_convex_polygon(
        line, NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTERSECT_LINE_CONVEX_POLYGON_POINTS_HPP_INCLUDED
