#pragma once

#include <stdexcept>

#include "cross_point.hpp"
#include "rotate90.hpp"

inline Point circumcenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    if(geometry_sign(cross(second - first, third - first)) == 0){
        throw std::invalid_argument("collinear triangle");
    }
    const Point first_midpoint = (first + second) / 2.0L;
    const Point second_midpoint = (first + third) / 2.0L;
    const Line first_bisector{
        first_midpoint,
        first_midpoint + rotate90(second - first),
    };
    const Line second_bisector{
        second_midpoint,
        second_midpoint + rotate90(third - first),
    };
    return cross_point(first_bisector, second_bisector);
}
