#pragma once

#include <stdexcept>

#include "cross.hpp"
#include "distance.hpp"

inline Point incenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    if(geometry_sign(cross(second - first, third - first)) == 0){
        throw std::invalid_argument("collinear triangle");
    }
    const long double opposite_first = distance(second, third);
    const long double opposite_second = distance(third, first);
    const long double opposite_third = distance(first, second);
    return (
        first * opposite_first +
        second * opposite_second +
        third * opposite_third
    ) / (opposite_first + opposite_second + opposite_third);
}
