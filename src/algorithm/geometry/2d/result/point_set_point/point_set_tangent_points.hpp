#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_POINT_SET_TANGENT_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_POINT_SET_TANGENT_POINTS_HPP_INCLUDED

#include <array>
#include <optional>
#include <utility>
#include <vector>

#include "../../point_set/point_set/convex_hull.hpp"
#include "../point/convex_polygon_tangents_normalized.hpp"

inline std::optional<std::array<Point, 2>> point_set_tangent_points(
    std::vector<Point> points,
    const Point& source
){
    NormalizedConvexPolygon hull(convex_hull(std::move(points)));
    if(hull.vertices().empty()) return std::nullopt;
    const ConvexPolygonTangentResult result = convex_polygon_tangents(hull, source);
    return std::array<Point, 2>{
        hull.vertices()[result.left], hull.vertices()[result.right]
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_POINT_SET_TANGENT_POINTS_HPP_INCLUDED
