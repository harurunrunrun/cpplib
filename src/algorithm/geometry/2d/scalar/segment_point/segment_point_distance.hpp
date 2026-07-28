#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_POINT_SEGMENT_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_POINT_SEGMENT_POINT_DISTANCE_HPP_INCLUDED

#include <cmath>

#include "../point/abs.hpp"
#include "../point_point/cross.hpp"
#include "../point_point/dot.hpp"

inline long double distance_segment_point(
    const Segment& segment,
    const Point& point
){
    const Point direction = segment.b - segment.a;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0){
        return abs(point - segment.a);
    }
    const Point unit_direction = direction / length;
    const long double projection = dot(point - segment.a, unit_direction);
    if(geometry_sign(projection) < 0){
        return abs(point - segment.a);
    }
    if(geometry_sign(projection - length) > 0){
        return abs(point - segment.b);
    }
    return std::abs(cross(unit_direction, point - segment.a));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_POINT_SEGMENT_POINT_DISTANCE_HPP_INCLUDED
