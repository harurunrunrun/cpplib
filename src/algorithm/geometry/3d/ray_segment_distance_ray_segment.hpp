#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_DISTANCE_RAY_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_DISTANCE_RAY_SEGMENT_HPP_INCLUDED

#include "abs.hpp"
#include "geometry_primitives.hpp"
#include "ray_segment_closest_points_ray_segment.hpp"

inline long double distance(const Ray3& ray, const Segment3& segment){
    const auto points = closest_points(ray, segment);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_DISTANCE_RAY_SEGMENT_HPP_INCLUDED
