#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_INTERSECT_LINE_CONVEX_POLYGON_NORMALIZED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_INTERSECT_LINE_CONVEX_POLYGON_NORMALIZED_HPP_INCLUDED

#include <stdexcept>
#include <vector>

#include "../../scalar/point/abs.hpp"
#include "../../index/point/convex_polygon_support_vertex_normalized.hpp"
#include "../../scalar/point_point/cross.hpp"
#include "../../point/point/rotate90.hpp"

inline bool intersect_line_convex_polygon(
    const Line& line,
    const NormalizedConvexPolygon& polygon
){
    const Point raw_direction = line.b - line.a;
    const long double direction_length = abs(raw_direction);
    if(geometry_sign(direction_length) == 0){
        throw std::invalid_argument("degenerate line");
    }
    const Point direction = raw_direction / direction_length;
    if(polygon.empty()) return false;
    const Point normal = rotate90(direction);
    const std::size_t maximum_index =
        convex_polygon_support_vertex(polygon, normal);
    const std::size_t minimum_index =
        convex_polygon_support_vertex(polygon, -normal);
    const std::vector<Point>& vertices = polygon.vertices();
    const long double maximum = cross(
        direction, vertices[maximum_index] - line.a
    );
    const long double minimum = cross(
        direction, vertices[minimum_index] - line.a
    );
    return geometry_sign(minimum) <= 0 && geometry_sign(maximum) >= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_INTERSECT_LINE_CONVEX_POLYGON_NORMALIZED_HPP_INCLUDED
