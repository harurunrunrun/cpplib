#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_LINE_CROSS_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_LINE_CROSS_POINTS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "norm.hpp"
#include "validate_circle.hpp"

inline std::vector<Point> circle_line_cross_points(
    const Circle& circle,
    const Line& line
){
    validate_circle(circle);
    const Point direction_vector = line.b - line.a;
    const long double direction_length = std::hypot(
        direction_vector.x, direction_vector.y
    );
    if(geometry_sign(direction_length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const long double direction_squared = direction_length * direction_length;
    const Point foot = line.a + direction_vector * (
        dot(circle.center - line.a, direction_vector) / direction_squared
    );
    const long double radius_squared = circle.radius * circle.radius;
    const long double foot_distance_squared = norm(foot - circle.center);
    const long double height_squared =
        radius_squared - foot_distance_squared;
    const int height_sign = circle_numeric_detail::scaled_sign(
        height_squared, radius_squared + foot_distance_squared
    );
    if(height_sign < 0) return {};
    if(height_sign == 0) return {foot};
    const Point direction = direction_vector / direction_length;
    const long double height =
        std::sqrt(std::max<long double>(0.0L, height_squared));
    std::vector<Point> result = {
        foot - direction * height,
        foot + direction * height,
    };
    std::sort(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_LINE_CROSS_POINTS_HPP_INCLUDED
