#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_POINT_ORIENT_PLANE_NORMAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_POINT_ORIENT_PLANE_NORMAL_HPP_INCLUDED

#include <stdexcept>

#include "../../scalar/point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"

inline Plane3 orient_plane_normal(
    Plane3 plane,
    const Point3& reference_direction
){
    if(geometry3d_sign(abs(plane.normal)) == 0)[[unlikely]]{
        throw std::invalid_argument("zero plane normal");
    }
    if(geometry3d_sign(abs(reference_direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("zero reference direction");
    }
    if(dot(plane.normal, reference_direction) < 0) plane.normal = -plane.normal;
    return plane;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_PLANE_POINT_ORIENT_PLANE_NORMAL_HPP_INCLUDED
