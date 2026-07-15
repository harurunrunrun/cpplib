#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"
#include "dot.hpp"
#include "plane3_unit_normal.hpp"
#include "is_finite.hpp"
#include "signed_distance.hpp"

inline long double sphere_cap_height(
    const Sphere3& sphere,
    const Plane3& cutting_plane,
    int side
){
    geometry3d_validate(sphere);
    if(side != -1 && side != 1)[[unlikely]]{
        throw std::invalid_argument("sphere cap side must be -1 or 1");
    }
    if(!geometry3d_is_finite(cutting_plane))[[unlikely]]{
        throw std::invalid_argument("sphere cap requires a finite cutting plane");
    }
    const Point3 normal = plane3_unit_normal(cutting_plane);
    const auto difference = geometry3d_normalized_difference(
        sphere.center, cutting_plane.point, {sphere.radius}
    );
    const long double scale = difference.scale;
    const long double radius = sphere.radius / scale;
    const Point3 center_offset = difference.value;
    const long double center_side = dot(center_offset, normal);
    const long double normalized_height = std::clamp(
        radius + side * center_side,
        0.0L,
        2 * radius
    );
    if(normalized_height > std::numeric_limits<long double>::max() / scale)
        [[unlikely]]{
        throw std::overflow_error("sphere cap height is not representable");
    }
    return normalized_height * scale;
}
