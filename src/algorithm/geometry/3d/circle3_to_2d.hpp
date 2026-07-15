#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../2d/types.hpp"
#include "abs.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "plane_coordinate_system.hpp"
#include "to_plane_coordinates.hpp"
#include "parallel.hpp"

inline Circle circle3_to_2d(
    const Circle3& circle,
    const PlaneCoordinateSystem3& system
){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    if(geometry3d_sign(abs(circle.normal)) == 0)[[unlikely]]{
        throw std::invalid_argument("zero circle normal");
    }
    if(!parallel(circle.normal, system.normal))[[unlikely]]{
        throw std::invalid_argument("circle plane and coordinate system differ");
    }
    if(std::abs(dot(circle.center - system.origin, system.normal))
        > GEOMETRY3D_EPS * std::max(1.0L, abs(circle.center - system.origin))
    )[[unlikely]]{
        throw std::invalid_argument("circle center is outside the coordinate plane");
    }
    return {to_plane_coordinates(system, circle.center), circle.radius};
}
