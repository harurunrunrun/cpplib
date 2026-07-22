#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "base.hpp"
#include "line_ray_closest_points.hpp"

inline long double distance(const Line3& line, const Ray3& ray){
    const auto points = closest_points(line, ray);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

inline long double distance(const Ray3& ray, const Line3& line){
    return distance(line, ray);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_HPP_INCLUDED
