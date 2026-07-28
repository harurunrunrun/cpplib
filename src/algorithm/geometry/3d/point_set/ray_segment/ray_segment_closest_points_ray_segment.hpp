#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_RAY_SEGMENT_RAY_SEGMENT_CLOSEST_POINTS_RAY_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_RAY_SEGMENT_RAY_SEGMENT_CLOSEST_POINTS_RAY_SEGMENT_HPP_INCLUDED

#include <utility>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../aggregate/all/projection.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_RAY_SEGMENT_RAY_SEGMENT_CLOSEST_POINTS_RAY_SEGMENT_HPP_INCLUDED
