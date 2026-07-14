#pragma once

#include <cstdint>
#include <vector>

#include "minimum_enclosing_circle.hpp"

inline std::vector<bool> minimum_enclosing_circle_boundary_flags(
    const std::vector<Point>& points,
    std::uint64_t seed = 0x9e3779b97f4a7c15ULL
){
    const MinimumEnclosingCircleResult circle =
        minimum_enclosing_circle(points, seed);
    std::vector<bool> result;
    result.reserve(points.size());
    for(const Point& point: points){
        result.push_back(circle.on_boundary(point));
    }
    return result;
}
