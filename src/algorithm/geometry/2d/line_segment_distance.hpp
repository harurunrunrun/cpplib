#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_SEGMENT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_SEGMENT_DISTANCE_HPP_INCLUDED

#include <algorithm>

#include "intersect_line_segment.hpp"
#include "line_point_distance.hpp"

inline long double distance_line_segment(
    const Line& line,
    const Segment& segment
){
    if(intersect_line_segment(line, segment)) return 0;
    return std::min(distance(line, segment.a), distance(line, segment.b));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_SEGMENT_DISTANCE_HPP_INCLUDED
