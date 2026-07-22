#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CIRCLE_SEGMENT_CROSS_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CIRCLE_SEGMENT_CROSS_POINTS_HPP_INCLUDED

#include <cmath>
#include <vector>

#include "circle_line_cross_points.hpp"
#include "../scalar/distance.hpp"
#include "../predicate/validate_circle.hpp"

inline std::vector<Point> circle_segment_cross_points(
    const Circle& circle,
    const Segment& segment
){
    validate_circle(circle);
    const Point direction = segment.b - segment.a;
    const long double direction_length = std::hypot(
        direction.x, direction.y
    );
    if(geometry_sign(direction_length) == 0){
        return circle_numeric_detail::compare(
            distance(circle.center, segment.a), circle.radius
        ) == 0
            ? std::vector<Point>{segment.a}
            : std::vector<Point>{};
    }
    const long double direction_squared = direction_length * direction_length;
    std::vector<Point> result;
    for(const Point& point: circle_line_cross_points(circle, segment)){
        const long double parameter =
            dot(point - segment.a, direction) / direction_squared;
        if(geometry_sign(parameter) >= 0
            && geometry_sign(parameter - 1.0L) <= 0){
            result.push_back(point);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_COLLECTION_CIRCLE_SEGMENT_CROSS_POINTS_HPP_INCLUDED
