#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LOWER_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LOWER_CONVEX_HULL_HPP_INCLUDED

#include <utility>
#include <vector>

#include "convex_hull_detail.hpp"

inline std::vector<Point> lower_convex_hull(
    std::vector<Point> points,
    bool keep_collinear = false
){
    points = convex_hull_detail::sorted_unique_points(std::move(points));
    return convex_hull_detail::build_chain_from_sorted(
        points, false, keep_collinear
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LOWER_CONVEX_HULL_HPP_INCLUDED
