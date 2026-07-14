#pragma once

#include <cmath>
#include <utility>
#include <vector>

#include "area.hpp"
#include "convex_hull.hpp"

inline long double convex_hull_area(std::vector<Point> points){
    return std::abs(area(convex_hull(std::move(points))));
}
