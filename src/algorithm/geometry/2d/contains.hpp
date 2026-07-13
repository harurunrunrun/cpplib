#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "cross.hpp"
#include "dot.hpp"

inline int contains(const std::vector<Point>& polygon, const Point& point){
    bool inside = false;
    const int size = static_cast<int>(polygon.size());
    for(int index = 0; index < size; ++index){
        Point first = polygon[static_cast<std::size_t>(index)] - point;
        Point second = polygon[
            static_cast<std::size_t>((index + 1) % size)
        ] - point;
        if(geometry_sign(cross(first, second)) == 0 &&
            geometry_sign(dot(first, second)) <= 0){
            return 1;
        }
        if(first.y > second.y) std::swap(first, second);
        if(geometry_sign(first.y) <= 0 && geometry_sign(second.y) > 0 &&
            geometry_sign(cross(first, second)) > 0){
            inside = !inside;
        }
    }
    return inside ? 2 : 0;
}
