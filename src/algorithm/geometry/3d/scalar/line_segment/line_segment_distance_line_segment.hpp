#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_SEGMENT_LINE_SEGMENT_DISTANCE_LINE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_SEGMENT_LINE_SEGMENT_DISTANCE_LINE_SEGMENT_HPP_INCLUDED

#include "../point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point_set/line_segment/line_segment_closest_points_line_segment.hpp"

inline long double distance(const Line3& line, const Segment3& segment){
    const auto points = closest_points(line, segment);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_SEGMENT_LINE_SEGMENT_DISTANCE_LINE_SEGMENT_HPP_INCLUDED
