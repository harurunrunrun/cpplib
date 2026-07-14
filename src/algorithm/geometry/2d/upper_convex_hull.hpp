#pragma once

#include <utility>
#include <vector>

#include "convex_hull_detail.hpp"

inline std::vector<Point> upper_convex_hull(
    std::vector<Point> points,
    bool keep_collinear = false
){
    points = convex_hull_detail::sorted_unique_points(std::move(points));
    return convex_hull_detail::build_chain_from_sorted(
        points, true, keep_collinear
    );
}
