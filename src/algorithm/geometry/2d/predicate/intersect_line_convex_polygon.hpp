#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_LINE_CONVEX_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_LINE_CONVEX_POLYGON_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "../scalar/abs.hpp"
#include "../query/convex_polygon_support_vertex.hpp"
#include "../scalar/cross.hpp"
#include "../point/rotate90.hpp"

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

inline bool intersect_line_convex_polygon(
    const Line& line,
    std::vector<Point> polygon
){
    return intersect_line_convex_polygon(
        line, NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_LINE_CONVEX_POLYGON_HPP_INCLUDED
