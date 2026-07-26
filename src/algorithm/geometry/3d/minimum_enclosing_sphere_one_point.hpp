#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINIMUM_ENCLOSING_SPHERE_ONE_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINIMUM_ENCLOSING_SPHERE_ONE_POINT_HPP_INCLUDED

#include "minimum_enclosing_sphere_detail.hpp"

inline Sphere3 minimum_enclosing_sphere(const Point3& point){
    minimum_enclosing_sphere_detail::validate_points(&point, 1);
    return {point, 0};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MINIMUM_ENCLOSING_SPHERE_ONE_POINT_HPP_INCLUDED
