#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "abs.hpp"
#include "convex_polygon_support_vertex.hpp"
#include "cross.hpp"
#include "rotate90.hpp"

inline bool intersect_line_convex_polygon(
    const Line& line,
    const NormalizedConvexPolygon& polygon
){
    const Point direction = line.b - line.a;
    if(geometry_sign(abs(direction)) == 0){
        throw std::invalid_argument("degenerate line");
    }
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
