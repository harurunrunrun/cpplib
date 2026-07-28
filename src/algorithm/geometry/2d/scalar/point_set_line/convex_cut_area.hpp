#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_LINE_CONVEX_CUT_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_LINE_CONVEX_CUT_AREA_HPP_INCLUDED

#include <cmath>
#include <vector>

#include "../point_set/area.hpp"
#include "../../point_set/point_set_line/convex_cut.hpp"

inline long double convex_cut_area(
    const std::vector<Point>& polygon,
    const Line& line
){
    return std::abs(area(convex_cut(polygon, line)));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_SET_LINE_CONVEX_CUT_AREA_HPP_INCLUDED
