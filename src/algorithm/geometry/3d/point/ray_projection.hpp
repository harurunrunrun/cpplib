#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY_PROJECTION_HPP_INCLUDED

#include "../core/geometry_primitives.hpp"
#include "closest_point.hpp"

inline Point3 projection(const Ray3& ray, const Point3& point){
    return closest_point(ray, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_RAY_PROJECTION_HPP_INCLUDED
