#pragma once

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
