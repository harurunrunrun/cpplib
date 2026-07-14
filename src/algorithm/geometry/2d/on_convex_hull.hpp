#pragma once

#include <vector>

#include "convex_polygon_query.hpp"

inline bool on_convex_hull(
    const ConvexPolygonQuery& polygon,
    const Point& point
){
    return polygon.contains(point) == 1;
}

inline bool on_convex_hull(
    const std::vector<Point>& polygon,
    const Point& point
){
    return on_convex_hull(ConvexPolygonQuery(polygon), point);
}
