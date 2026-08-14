#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POLYGON_DIAMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POLYGON_DIAMETER_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../../point_set/point_set/convex_hull.hpp"
#include "convex_diameter.hpp"

inline long double polygon_diameter(std::vector<Point> polygon){
    return convex_diameter(convex_hull(std::move(polygon)));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_POLYGON_DIAMETER_HPP_INCLUDED
