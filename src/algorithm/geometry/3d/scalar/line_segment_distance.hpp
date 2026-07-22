#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_SEGMENT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_SEGMENT_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../point_collection/line_segment_closest_points.hpp"

inline long double distance(const Line3& line, const Segment3& segment){
    const auto points = closest_points(line, segment);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

inline long double distance(const Segment3& segment, const Line3& line){
    return distance(line, segment);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_SEGMENT_DISTANCE_HPP_INCLUDED
