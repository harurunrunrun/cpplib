#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_POINTS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "convex_polygon_maximum_area_triangle_normalized.hpp"

inline ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(std::vector<Point> polygon){
    return convex_polygon_maximum_area_triangle(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_POINTS_HPP_INCLUDED
