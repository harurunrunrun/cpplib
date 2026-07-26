#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_DISTANCE_PLANE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_DISTANCE_PLANE_RAY_HPP_INCLUDED

#include "ray_plane_distance_ray_plane.hpp"

inline long double distance(const Plane3& plane, const Ray3& ray){
    return distance(ray, plane);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_PLANE_DISTANCE_PLANE_RAY_HPP_INCLUDED
