#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_SEGMENT_PLANE_SEGMENT_PLANE_CLOSEST_POINTS_SEGMENT_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_SEGMENT_PLANE_SEGMENT_PLANE_CLOSEST_POINTS_SEGMENT_PLANE_HPP_INCLUDED

#include <utility>

#include "../../scalar/point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../aggregate/all/projection.hpp"
#include "../../result/segment_plane/segment_plane_intersection.hpp"
#include "../../scalar/plane_point/signed_distance.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Plane3& plane
){
    const auto intersection = segment_plane_intersection(segment, plane);
    if(intersection) return {*intersection, *intersection};
    const Point3 endpoint =
        std::abs(signed_distance(plane, segment.a)) <=
        std::abs(signed_distance(plane, segment.b))
        ? segment.a : segment.b;
    return {endpoint, projection(plane, endpoint)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_SEGMENT_PLANE_SEGMENT_PLANE_CLOSEST_POINTS_SEGMENT_PLANE_HPP_INCLUDED
