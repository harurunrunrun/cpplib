#pragma once

#include <algorithm>
#include <vector>

#include "area.hpp"

inline std::vector<Point> counterclockwise_polygon(std::vector<Point> polygon){
    if(geometry_sign(area(polygon)) < 0){
        std::reverse(polygon.begin(), polygon.end());
    }
    return polygon;
}
