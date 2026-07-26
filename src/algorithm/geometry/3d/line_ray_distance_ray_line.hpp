#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_RAY_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_RAY_LINE_HPP_INCLUDED

#include "line_ray_distance_line_ray.hpp"

inline long double distance(const Ray3& ray, const Line3& line){
    return distance(line, ray);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_DISTANCE_RAY_LINE_HPP_INCLUDED
