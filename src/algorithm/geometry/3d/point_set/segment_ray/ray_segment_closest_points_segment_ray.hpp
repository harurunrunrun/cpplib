#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_SEGMENT_RAY_RAY_SEGMENT_CLOSEST_POINTS_SEGMENT_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_SEGMENT_RAY_RAY_SEGMENT_CLOSEST_POINTS_SEGMENT_RAY_HPP_INCLUDED

#include <utility>

#include "../ray_segment/ray_segment_closest_points_ray_segment.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Ray3& ray
){
    const auto result = closest_points(ray, segment);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_SEGMENT_RAY_RAY_SEGMENT_CLOSEST_POINTS_SEGMENT_RAY_HPP_INCLUDED
