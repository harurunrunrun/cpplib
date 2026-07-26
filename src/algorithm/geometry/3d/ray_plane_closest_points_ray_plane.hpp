#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_CLOSEST_POINTS_RAY_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_CLOSEST_POINTS_RAY_PLANE_HPP_INCLUDED

#include <utility>

#include "geometry_primitives.hpp"
#include "projection.hpp"
#include "ray3_direction.hpp"
#include "ray_plane_intersection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& ray,
    const Plane3& plane
){
    (void)ray3_direction(ray);
    const auto intersection = ray_plane_intersection(ray, plane);
    if(intersection) return {*intersection, *intersection};
    return {ray.origin, projection(plane, ray.origin)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_CLOSEST_POINTS_RAY_PLANE_HPP_INCLUDED
