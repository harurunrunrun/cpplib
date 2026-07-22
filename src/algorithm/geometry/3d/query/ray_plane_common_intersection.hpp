#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_RAY_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_RAY_PLANE_COMMON_INTERSECTION_HPP_INCLUDED

#include "../predicate/is_finite.hpp"
#include "../point/line_plane_intersection.hpp"
#include "linear_intersection3.hpp"
#include "../predicate/on_plane.hpp"

inline LinearIntersection3 ray_plane_common_intersection(
    const Ray3& ray,
    const Plane3& plane
){
    if(!geometry3d_is_finite(ray) || !geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument(
            "ray_plane_common_intersection requires finite inputs"
        );
    }
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        ray.origin, ray.through, plane
    );
    if(!intersection){
        if(on_plane(plane, ray.origin)) return ray;
        return std::monostate{};
    }
    if(geometry3d_line_plane_detail::parameter_sign(*intersection) < 0){
        return std::monostate{};
    }
    return intersection->point;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_RAY_PLANE_COMMON_INTERSECTION_HPP_INCLUDED
