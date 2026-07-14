#pragma once

#include <algorithm>
#include <stdexcept>

#include "advanced/detail.hpp"

inline Point incenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    if(advanced_geometry_detail::cross_sign(
        second - first, third - first
    ) == 0){
        throw std::invalid_argument("collinear triangle");
    }
    const long double opposite_first =
        advanced_geometry_detail::length(second - third);
    const long double opposite_second =
        advanced_geometry_detail::length(third - first);
    const long double opposite_third =
        advanced_geometry_detail::length(first - second);
    const long double weight_scale = std::max({
        opposite_first, opposite_second, opposite_third
    });
    const long double normalized_first = opposite_first / weight_scale;
    const long double normalized_second = opposite_second / weight_scale;
    const long double normalized_third = opposite_third / weight_scale;
    const long double normalized_sum =
        normalized_first + normalized_second + normalized_third;
    return first +
        (second - first) * (normalized_second / normalized_sum) +
        (third - first) * (normalized_third / normalized_sum);
}
