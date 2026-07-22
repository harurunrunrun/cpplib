#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "base.hpp"
#include "ray_ray_closest_points.hpp"

inline long double distance(const Ray3& first, const Ray3& second){
    const auto points = closest_points(first, second);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_DISTANCE_HPP_INCLUDED
