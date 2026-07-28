#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_RAY_LINE_RAY_DISTANCE_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_RAY_LINE_RAY_DISTANCE_LINE_RAY_HPP_INCLUDED

#include "../point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point_set/line_ray/line_ray_closest_points_line_ray.hpp"

inline long double distance(const Line3& line, const Ray3& ray){
    const auto points = closest_points(line, ray);
    return geometry3d_linear_closest_detail::checked_distance(
        points.first, points.second
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_RAY_LINE_RAY_DISTANCE_LINE_RAY_HPP_INCLUDED
