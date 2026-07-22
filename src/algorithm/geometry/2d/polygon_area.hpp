#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_AREA_HPP_INCLUDED

#include <cmath>

#include "area.hpp"

inline long double polygon_area(const std::vector<Point>& polygon){
    return std::abs(area(polygon));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_AREA_HPP_INCLUDED
