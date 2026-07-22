#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SIGNED_CIRCLE_POLYGON_INTERSECTION_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SIGNED_CIRCLE_POLYGON_INTERSECTION_AREA_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../detail/circle_polygon_intersection_area_detail.hpp"
#include "../predicate/validate_circle.hpp"

inline long double signed_circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
){
    validate_circle(circle);
    if(circle.radius == 0.0L || polygon.size() < 2) return 0.0L;
    long double result = 0.0L;
    for(std::size_t i = 0; i < polygon.size(); i++){
        const Point a = polygon[i] - circle.center;
        const Point b = polygon[(i + 1) % polygon.size()] - circle.center;
        result += circle_polygon_intersection_internal::edge_area(a, b, circle.radius);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SIGNED_CIRCLE_POLYGON_INTERSECTION_AREA_HPP_INCLUDED
