#pragma once

#include <utility>
#include <vector>

#include "convex_polygon_minimum_distance.hpp"

inline bool intersect_convex_polygons(
    std::vector<Point> first,
    std::vector<Point> second
){
    if(first.empty() || second.empty()) return false;
    return geometry_sign(convex_polygon_minimum_distance(
        std::move(first), std::move(second)
    )) == 0;
}
