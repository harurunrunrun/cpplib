#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_PLANE_CUT_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_PLANE_CUT_VOLUME_HPP_INCLUDED

#include "base.hpp"
#include "sphere_cap_height.hpp"
#include "sphere_cap_volume.hpp"

inline long double sphere_plane_cut_volume(
    const Sphere3& sphere,
    const Plane3& cutting_plane,
    int side
){
    return sphere_cap_volume(
        sphere,
        sphere_cap_height(sphere, cutting_plane, side)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_PLANE_CUT_VOLUME_HPP_INCLUDED
