#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_DISTANCE_LINE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_DISTANCE_LINE_SEGMENT_HPP_INCLUDED

#include "abs.hpp"
#include "geometry_primitives.hpp"
#include "line_segment_closest_points_line_segment.hpp"

inline long double distance(const Line3& line, const Segment3& segment){
    const auto points = closest_points(line, segment);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_DISTANCE_LINE_SEGMENT_HPP_INCLUDED
