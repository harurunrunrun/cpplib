#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"

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
