#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_UPPER_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_UPPER_CONVEX_HULL_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../detail/convex_hull_construction_detail.hpp"

inline std::vector<Point> upper_convex_hull(
    std::vector<Point> points,
    bool keep_collinear = false
){
    points = convex_hull_detail::sorted_unique_points(std::move(points));
    return convex_hull_detail::build_chain_from_sorted(
        points, true, keep_collinear
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_UPPER_CONVEX_HULL_HPP_INCLUDED
