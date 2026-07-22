#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_CLOSEST_POINT_ON_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_CLOSEST_POINT_ON_SEGMENT_HPP_INCLUDED

#include <algorithm>

#include "../scalar/abs.hpp"
#include "../scalar/dot.hpp"
#include "../scalar/norm.hpp"

inline Point closest_point_on_segment(
    const Segment& segment,
    const Point& point
){
    const Point direction = segment.b - segment.a;
    const long double squared_length = norm(direction);
    if(geometry_sign(abs(direction)) == 0) return segment.a;
    const long double parameter = std::clamp(
        dot(point - segment.a, direction) / squared_length,
        0.0L,
        1.0L
    );
    return segment.a + direction * parameter;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_CLOSEST_POINT_ON_SEGMENT_HPP_INCLUDED
