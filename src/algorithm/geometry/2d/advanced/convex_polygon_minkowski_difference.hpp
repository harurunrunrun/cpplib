#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_CONVEX_POLYGON_MINKOWSKI_DIFFERENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_CONVEX_POLYGON_MINKOWSKI_DIFFERENCE_HPP_INCLUDED

#include <utility>
#include <vector>

#include "convex_polygon_minkowski_sum.hpp"

inline std::vector<Point> convex_polygon_minkowski_difference(
    std::vector<Point> first,
    std::vector<Point> second
){
    for(Point& point: second) point = -point;
    return convex_polygon_minkowski_sum(
        std::move(first), std::move(second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_CONVEX_POLYGON_MINKOWSKI_DIFFERENCE_HPP_INCLUDED
