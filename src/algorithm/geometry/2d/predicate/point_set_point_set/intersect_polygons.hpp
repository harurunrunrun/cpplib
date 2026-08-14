#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_SET_POINT_SET_INTERSECT_POLYGONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_SET_POINT_SET_INTERSECT_POLYGONS_HPP_INCLUDED

#include <vector>

#include "../../aggregate/all/geometry_sign.hpp"
#include "../../scalar/point_set_point_set/polygon_minimum_distance.hpp"

inline bool intersect_polygons(
    const std::vector<Point>& first,
    const std::vector<Point>& second
){
    if(first.empty() || second.empty()) return false;
    return geometry_sign(polygon_minimum_distance(first, second)) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_SET_POINT_SET_INTERSECT_POLYGONS_HPP_INCLUDED
