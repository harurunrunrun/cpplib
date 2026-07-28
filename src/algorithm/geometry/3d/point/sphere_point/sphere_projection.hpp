#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERE_POINT_SPHERE_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERE_POINT_SPHERE_PROJECTION_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "sphere_closest_point.hpp"

inline Point3 projection(const Sphere3& sphere, const Point3& point){
    return closest_point(sphere, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERE_POINT_SPHERE_PROJECTION_HPP_INCLUDED
