#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PROPER_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PROPER_SEGMENT_INTERSECTION_HPP_INCLUDED

#include "abs.hpp"
#include "side_of_directed_line.hpp"

inline bool proper_segment_intersection(
    const Segment& first,
    const Segment& second
){
    const auto side = [](const Segment& segment, const Point& point){
        if(geometry_sign(abs(segment.b - segment.a)) == 0) return 0;
        return side_of_directed_line(segment, point);
    };
    const int first_a = side(first, second.a);
    const int first_b = side(first, second.b);
    const int second_a = side(second, first.a);
    const int second_b = side(second, first.b);
    return first_a * first_b < 0 && second_a * second_b < 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PROPER_SEGMENT_INTERSECTION_HPP_INCLUDED
