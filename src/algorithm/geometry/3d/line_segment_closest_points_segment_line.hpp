#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_CLOSEST_POINTS_SEGMENT_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_CLOSEST_POINTS_SEGMENT_LINE_HPP_INCLUDED

#include <utility>

#include "line_segment_closest_points_line_segment.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Line3& line
){
    const auto result = closest_points(line, segment);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_CLOSEST_POINTS_SEGMENT_LINE_HPP_INCLUDED
