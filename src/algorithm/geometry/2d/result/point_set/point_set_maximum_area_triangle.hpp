#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_MAXIMUM_AREA_TRIANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_MAXIMUM_AREA_TRIANGLE_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../point_set/point_set/convex_hull.hpp"
#include "../../result/mixed/convex_polygon_maximum_area_triangle_normalized.hpp"
#include "../../type/definition/point_set_maximum_area_triangle_result.hpp"

inline PointSetMaximumAreaTriangleResult point_set_maximum_area_triangle(
    std::vector<Point> points
){
    NormalizedConvexPolygon hull(convex_hull(std::move(points)));
    const ConvexPolygonMaximumAreaTriangleResult result =
        convex_polygon_maximum_area_triangle(hull);
    if(!result.valid()) return {};
    const std::vector<Point>& vertices = hull.vertices();
    return {{vertices[result.indices[0]], vertices[result.indices[1]],
        vertices[result.indices[2]]}, result.doubled_area, true};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_MAXIMUM_AREA_TRIANGLE_HPP_INCLUDED
