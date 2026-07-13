#pragma once

#include <vector>

#include "convex_polygon_query.hpp"

inline int contains_convex(
    const std::vector<Point>& polygon,
    const Point& point
){
    return ConvexPolygonQuery(polygon).contains(point);
}
