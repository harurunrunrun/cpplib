#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_TRIANGLE_INTERSECTION_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_TRIANGLE_INTERSECTION_AREA_HPP_INCLUDED

#include <vector>

#include "circle_polygon_intersection_area.hpp"

inline long double circle_triangle_intersection_area(
    const Circle& circle,
    const Point& first,
    const Point& second,
    const Point& third
){
    return circle_polygon_intersection_area(
        circle, std::vector<Point>{first, second, third}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_TRIANGLE_INTERSECTION_AREA_HPP_INCLUDED
