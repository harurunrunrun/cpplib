#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_POLYGON_MAXIMUM_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_POLYGON_MAXIMUM_DISTANCE_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "../../point_set/point_set/convex_hull.hpp"
#include "convex_polygon_maximum_distance.hpp"

inline long double polygon_maximum_distance(
    std::vector<Point> first,
    std::vector<Point> second
){
    if(first.empty() || second.empty())[[unlikely]]{
        throw std::invalid_argument("maximum distance to an empty polygon");
    }
    return convex_polygon_maximum_distance(
        convex_hull(std::move(first)),
        convex_hull(std::move(second))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POINT_SET_POLYGON_MAXIMUM_DISTANCE_HPP_INCLUDED
