#pragma once

#include <cmath>

#include "area.hpp"

inline long double polygon_area(const std::vector<Point>& polygon){
    return std::abs(area(polygon));
}
