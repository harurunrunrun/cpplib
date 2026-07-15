#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"
#include "abs.hpp"
#include "geometry3d_sign.hpp"
#include "dot.hpp"
#include "plane3_unit_normal.hpp"
#include "on_plane.hpp"

inline bool on_circle(const Circle3& circle, const Point3& point){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("on_circle requires a finite point");
    }
    const Point3 normal = plane3_unit_normal({circle.center, circle.normal});
    const auto difference = geometry3d_normalized_difference(
        point, circle.center, {circle.radius}
    );
    const Point3 offset = difference.value;
    const long double center_distance = std::hypot(
        offset.x, offset.y, offset.z
    );
    if(geometry3d_scaled_sign(
        dot(offset, normal), center_distance
    ) != 0) return false;
    const long double radius = circle.radius / difference.scale;
    return geometry3d_scaled_sign(
        center_distance - radius, std::max(center_distance, radius)
    ) == 0;
}
