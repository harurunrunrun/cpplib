#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_CIRCLES_THROUGH_TWO_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_CIRCLES_THROUGH_TWO_POINTS_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "../scalar/distance.hpp"
#include "../point/rotate90.hpp"
#include "../predicate/validate_circle.hpp"

inline std::vector<Circle> circles_through_two_points(
    const Point& first,
    const Point& second,
    long double radius
){
    validate_circle({{}, radius});
    const long double chord_length = distance(first, second);
    if(chord_length == 0.0L){
        if(radius == 0.0L) return {{first, radius}};
        throw std::domain_error(
            "infinitely many circles pass through one point"
        );
    }
    if(circle_numeric_detail::compare(
        chord_length, 2.0L * radius
    ) > 0) return {};

    const Point midpoint = (first + second) / 2.0L;
    long double height_squared = radius * radius
        - chord_length * chord_length / 4.0L;
    const int height_sign = circle_numeric_detail::scaled_sign(
        height_squared,
        radius * radius + chord_length * chord_length / 4.0L
    );
    if(height_sign < 0) return {};
    if(height_sign == 0) return {{midpoint, radius}};
    height_squared = std::max(0.0L, height_squared);
    const Point offset = rotate90((second - first) / chord_length)
        * std::sqrt(height_squared);
    std::vector<Circle> result = {
        {midpoint - offset, radius},
        {midpoint + offset, radius},
    };
    std::sort(result.begin(), result.end(), [](const Circle& left, const Circle& right){
        return left.center < right.center;
    });
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_CIRCLES_THROUGH_TWO_POINTS_HPP_INCLUDED
