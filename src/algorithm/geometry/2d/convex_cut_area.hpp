#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_CUT_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_CUT_AREA_HPP_INCLUDED

#include <cmath>
#include <vector>

#include "area.hpp"
#include "convex_cut.hpp"

inline long double convex_cut_area(
    const std::vector<Point>& polygon,
    const Line& line
){
    return std::abs(area(convex_cut(polygon, line)));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_CUT_AREA_HPP_INCLUDED
