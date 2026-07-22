#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_PLANE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_PLANE_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../point_collection/ray_plane_closest_points.hpp"

inline long double distance(const Ray3& ray, const Plane3& plane){
    const auto points = closest_points(ray, plane);
    return abs(points.first - points.second);
}

inline long double distance(const Plane3& plane, const Ray3& ray){
    return distance(ray, plane);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_RAY_PLANE_DISTANCE_HPP_INCLUDED
