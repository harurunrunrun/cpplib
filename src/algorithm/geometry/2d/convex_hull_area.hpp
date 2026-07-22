#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_HULL_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_HULL_AREA_HPP_INCLUDED

#include <cmath>
#include <utility>
#include <vector>

#include "area.hpp"
#include "convex_hull.hpp"

inline long double convex_hull_area(std::vector<Point> points){
    return std::abs(area(convex_hull(std::move(points))));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_HULL_AREA_HPP_INCLUDED
