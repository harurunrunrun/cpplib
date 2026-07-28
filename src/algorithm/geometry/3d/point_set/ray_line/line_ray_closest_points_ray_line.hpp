#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_RAY_LINE_LINE_RAY_CLOSEST_POINTS_RAY_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_RAY_LINE_LINE_RAY_CLOSEST_POINTS_RAY_LINE_HPP_INCLUDED

#include <utility>

#include "../line_ray/line_ray_closest_points_line_ray.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& ray,
    const Line3& line
){
    const auto result = closest_points(line, ray);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_RAY_LINE_LINE_RAY_CLOSEST_POINTS_RAY_LINE_HPP_INCLUDED
