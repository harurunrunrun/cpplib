#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_PROJECTION_HPP_INCLUDED

#include "base.hpp"
#include "sphere_closest_point.hpp"

inline Point3 projection(const Sphere3& sphere, const Point3& point){
    return closest_point(sphere, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_PROJECTION_HPP_INCLUDED
