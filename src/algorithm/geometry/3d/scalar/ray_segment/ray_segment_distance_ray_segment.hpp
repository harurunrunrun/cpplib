#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_SEGMENT_RAY_SEGMENT_DISTANCE_RAY_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_SEGMENT_RAY_SEGMENT_DISTANCE_RAY_SEGMENT_HPP_INCLUDED

#include "../point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point_set/ray_segment/ray_segment_closest_points_ray_segment.hpp"

inline long double distance(const Ray3& ray, const Segment3& segment){
    const auto points = closest_points(ray, segment);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_SEGMENT_RAY_SEGMENT_DISTANCE_RAY_SEGMENT_HPP_INCLUDED
