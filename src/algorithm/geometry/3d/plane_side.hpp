#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_SIDE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_SIDE_HPP_INCLUDED

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "signed_distance.hpp"

inline int plane_side(const Plane3& plane, const Point3& point){
    return geometry3d_sign(signed_distance(plane, point));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_SIDE_HPP_INCLUDED
