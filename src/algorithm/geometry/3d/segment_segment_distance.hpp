#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_DISTANCE_HPP_INCLUDED

#include "closest_points.hpp"
#include "linear_closest_detail.hpp"
#include "segment3.hpp"

inline long double distance(const Segment3& first, const Segment3& second){
    const auto [left, right] = closest_points(first, second);
    return geometry3d_linear_closest_detail::checked_distance(left, right);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_DISTANCE_HPP_INCLUDED
