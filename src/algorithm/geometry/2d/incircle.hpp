#pragma once

#include <cmath>

#include "advanced/detail.hpp"
#include "incenter.hpp"

inline Circle incircle(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Point center = incenter(first, second, third);
    const Point direction = second - first;
    const long double length = advanced_geometry_detail::length(direction);
    const long double radius = std::abs(cross(direction / length, center - first));
    return {center, radius};
}
