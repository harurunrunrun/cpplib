#pragma once

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "signed_distance.hpp"

inline int plane_side(const Plane3& plane, const Point3& point){
    return geometry3d_sign(signed_distance(plane, point));
}
