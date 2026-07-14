#pragma once

#include <utility>
#include <vector>

#include "convex_hull_detail.hpp"

inline std::vector<Point> convex_hull_with_collinear(std::vector<Point> points){
    return convex_hull_detail::build_convex_hull(std::move(points), true);
}
