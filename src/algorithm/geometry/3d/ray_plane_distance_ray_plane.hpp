#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_DISTANCE_RAY_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_DISTANCE_RAY_PLANE_HPP_INCLUDED

#include "abs.hpp"
#include "geometry_primitives.hpp"
#include "ray_plane_closest_points_ray_plane.hpp"

inline long double distance(const Ray3& ray, const Plane3& plane){
    const auto points = closest_points(ray, plane);
    return abs(points.first - points.second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_DISTANCE_RAY_PLANE_HPP_INCLUDED
