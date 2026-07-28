#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_CIRCLE_SEGMENT_CIRCLE_SEGMENT_INTERSECTION_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_CIRCLE_SEGMENT_CIRCLE_SEGMENT_INTERSECTION_COUNT_HPP_INCLUDED

#include "../../point_set/circle_segment/circle_segment_cross_points.hpp"

inline int circle_segment_intersection_count(
    const Circle& circle,
    const Segment& segment
){
    return static_cast<int>(
        circle_segment_cross_points(circle, segment).size()
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_CIRCLE_SEGMENT_CIRCLE_SEGMENT_INTERSECTION_COUNT_HPP_INCLUDED
