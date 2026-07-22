#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCUMCENTER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCUMCENTER_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>

#include "advanced/detail.hpp"

inline Point circumcenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point first_direction = second - first;
    const Point second_direction = third - first;
    if(advanced_geometry_detail::cross_sign(
        first_direction, second_direction
    ) == 0){
        throw std::invalid_argument("collinear triangle");
    }

    const long double scale = std::max(
        advanced_geometry_detail::length(first_direction),
        advanced_geometry_detail::length(second_direction)
    );
    const Point first_scaled = first_direction / scale;
    const Point second_scaled = second_direction / scale;
    const long double first_norm = dot(first_scaled, first_scaled);
    const long double second_norm = dot(second_scaled, second_scaled);
    const long double denominator =
        2.0L * cross(first_scaled, second_scaled);
    const Point offset{
        (
            second_scaled.y * first_norm -
            first_scaled.y * second_norm
        ) / denominator,
        (
            first_scaled.x * second_norm -
            second_scaled.x * first_norm
        ) / denominator,
    };
    return first + offset * scale;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCUMCENTER_HPP_INCLUDED
