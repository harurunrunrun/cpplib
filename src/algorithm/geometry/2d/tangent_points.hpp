#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TANGENT_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TANGENT_POINTS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <vector>

#include "norm.hpp"
#include "rotate90.hpp"
#include "validate_circle.hpp"

inline std::vector<Point> tangent_points(
    const Circle& circle,
    const Point& point
){
    validate_circle(circle);
    if(circle.radius == 0.0L) return {circle.center};
    const Point offset_from_center = point - circle.center;
    const long double distance_squared = norm(offset_from_center);
    const long double radius_squared = circle.radius * circle.radius;
    const int distance_sign = circle_numeric_detail::scaled_sign(
        distance_squared - radius_squared, distance_squared + radius_squared
    );
    if(distance_sign < 0) return {};
    if(distance_sign == 0) return {point};
    const Point base = circle.center +
        offset_from_center * (radius_squared / distance_squared);
    const Point perpendicular = rotate90(offset_from_center) * (
        circle.radius * std::sqrt(std::max<long double>(
            0.0L,
            distance_squared - radius_squared
        )) / distance_squared
    );
    std::vector<Point> result = {
        base - perpendicular,
        base + perpendicular,
    };
    std::sort(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TANGENT_POINTS_HPP_INCLUDED
