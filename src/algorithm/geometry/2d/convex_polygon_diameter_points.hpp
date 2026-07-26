#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_POINTS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "convex_polygon_diameter_normalized.hpp"

inline ConvexPolygonDiameterResult convex_polygon_diameter(
    std::vector<Point> polygon
){
    return convex_polygon_diameter(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_POINTS_HPP_INCLUDED
