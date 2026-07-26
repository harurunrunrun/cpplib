#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_SUPPORT_VERTEX_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_SUPPORT_VERTEX_POINTS_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "convex_polygon_support_vertex_normalized.hpp"

inline std::size_t convex_polygon_support_vertex(
    std::vector<Point> polygon,
    const Point& direction
){
    return convex_polygon_support_vertex(
        NormalizedConvexPolygon(std::move(polygon)), direction
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_SUPPORT_VERTEX_POINTS_HPP_INCLUDED
