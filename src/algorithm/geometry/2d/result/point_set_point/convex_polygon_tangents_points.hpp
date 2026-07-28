#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_CONVEX_POLYGON_TANGENTS_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_CONVEX_POLYGON_TANGENTS_POINTS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../point/convex_polygon_tangents_normalized.hpp"

inline ConvexPolygonTangentResult convex_polygon_tangents(
    std::vector<Point> polygon,
    const Point& point
){
    return convex_polygon_tangents(
        NormalizedConvexPolygon(std::move(polygon)), point
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_CONVEX_POLYGON_TANGENTS_POINTS_HPP_INCLUDED
