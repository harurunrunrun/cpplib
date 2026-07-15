#pragma once

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
