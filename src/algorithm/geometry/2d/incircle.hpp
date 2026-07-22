#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INCIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INCIRCLE_HPP_INCLUDED

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INCIRCLE_HPP_INCLUDED
