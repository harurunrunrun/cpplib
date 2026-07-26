#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_LINE_RAY_HPP_INCLUDED

#include "abs.hpp"
#include "geometry_primitives.hpp"
#include "line_ray_closest_points_line_ray.hpp"

inline long double distance(const Line3& line, const Ray3& ray){
    const auto points = closest_points(line, ray);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_LINE_RAY_HPP_INCLUDED
