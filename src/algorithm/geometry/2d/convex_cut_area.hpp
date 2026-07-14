#pragma once

#include <cmath>
#include <vector>

#include "area.hpp"
#include "convex_cut.hpp"

inline long double convex_cut_area(
    const std::vector<Point>& polygon,
    const Line& line
){
    return std::abs(area(convex_cut(polygon, line)));
}
