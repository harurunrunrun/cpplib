#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_CLOSEST_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_CLOSEST_POINTS_HPP_INCLUDED

#include <utility>

#include "base.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& ray,
    const Segment3& segment
){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {ray.origin, ray.through, Kind::ray},
        {segment.a, segment.b, Kind::segment}
    );
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Ray3& ray
){
    const auto result = closest_points(ray, segment);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_CLOSEST_POINTS_HPP_INCLUDED
