#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_INTERSECTION_HPP_INCLUDED

#include <optional>

#include "base.hpp"
#include "line_plane_intersection.hpp"

inline std::optional<Point3> ray_plane_intersection(
    const Ray3& ray,
    const Plane3& plane
){
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        ray.origin, ray.through, plane
    );
    if(!intersection
        || geometry3d_line_plane_detail::parameter_sign(*intersection) < 0){
        return std::nullopt;
    }
    return intersection->point;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_INTERSECTION_HPP_INCLUDED
