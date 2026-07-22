#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POLYGON_INTERSECTION_AREA_CIRCLE_POLYGON_INTERSECTION_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POLYGON_INTERSECTION_AREA_CIRCLE_POLYGON_INTERSECTION_AREA_HPP_INCLUDED

#include <cmath>
#include <vector>

#include "signed_circle_polygon_intersection_area.hpp"

inline long double circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
){
    return std::abs(signed_circle_polygon_intersection_area(circle, polygon));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_POLYGON_INTERSECTION_AREA_CIRCLE_POLYGON_INTERSECTION_AREA_HPP_INCLUDED
