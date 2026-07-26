#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_FARTHEST_VERTICES_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_FARTHEST_VERTICES_POINTS_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "convex_polygon_farthest_vertices_normalized.hpp"

inline std::vector<std::size_t> convex_polygon_farthest_vertices(
    std::vector<Point> polygon
){
    return convex_polygon_farthest_vertices(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_FARTHEST_VERTICES_POINTS_HPP_INCLUDED
