#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_DISTANCE_SEGMENT_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_DISTANCE_SEGMENT_LINE_HPP_INCLUDED

#include "line_segment_distance_line_segment.hpp"

inline long double distance(const Segment3& segment, const Line3& line){
    return distance(line, segment);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_DISTANCE_SEGMENT_LINE_HPP_INCLUDED
