#pragma once

#include <stdexcept>
#include <vector>

#include "cross.hpp"

inline Point polygon_centroid(const std::vector<Point>& polygon){
    long double doubled_area = 0.0L;
    Point weighted{};
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& current = polygon[index];
        const Point& next = polygon[(index + 1) % polygon.size()];
        const long double determinant = cross(current, next);
        doubled_area += determinant;
        weighted += (current + next) * determinant;
    }
    if(geometry_sign(doubled_area) == 0)[[unlikely]]{
        throw std::invalid_argument("polygon has zero signed area");
    }
    return weighted / (3.0L * doubled_area);
}
