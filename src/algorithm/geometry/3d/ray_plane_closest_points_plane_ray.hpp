#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_CLOSEST_POINTS_PLANE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_CLOSEST_POINTS_PLANE_RAY_HPP_INCLUDED

#include <utility>

#include "ray_plane_closest_points_ray_plane.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Plane3& plane,
    const Ray3& ray
){
    const auto result = closest_points(ray, plane);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_CLOSEST_POINTS_PLANE_RAY_HPP_INCLUDED
