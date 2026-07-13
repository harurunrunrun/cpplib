#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "norm.hpp"
#include "plane3_unit_normal.hpp"

inline Line3 plane_plane_intersection(
    const Plane3& first,
    const Plane3& second
){
    const Point3 first_normal = plane3_unit_normal(first);
    const Point3 second_normal = plane3_unit_normal(second);
    const Point3 direction = cross(first_normal, second_normal);
    const long double denominator = norm(direction);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::domain_error("3D planes do not have a unique intersection line");
    }
    const long double first_offset = dot(first_normal, first.point);
    const long double second_offset = dot(second_normal, second.point);
    const Point3 point = cross(
        direction,
        second_offset * first_normal - first_offset * second_normal
    ) / denominator;
    return {point, point + direction};
}
