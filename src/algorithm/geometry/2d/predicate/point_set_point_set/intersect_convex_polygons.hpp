#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_SET_POINT_SET_INTERSECT_CONVEX_POLYGONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_SET_POINT_SET_INTERSECT_CONVEX_POLYGONS_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../scalar/point_set_point_set/convex_polygon_minimum_distance.hpp"

inline bool intersect_convex_polygons(
    std::vector<Point> first,
    std::vector<Point> second
){
    if(first.empty() || second.empty()) return false;
    return geometry_sign(convex_polygon_minimum_distance(
        std::move(first), std::move(second)
    )) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_SET_POINT_SET_INTERSECT_CONVEX_POLYGONS_HPP_INCLUDED
