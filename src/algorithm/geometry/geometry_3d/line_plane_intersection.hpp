#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line3_direction.hpp"
#include "plane3_unit_normal.hpp"

inline Point3 line_plane_intersection(const Line3& line, const Plane3& plane){
    const Point3 direction = line3_direction(line);
    const Point3 normal = plane3_unit_normal(plane);
    const long double denominator = dot(direction, normal);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::domain_error("3D line and plane do not have a unique intersection");
    }
    const long double parameter = dot(plane.point - line.a, normal) / denominator;
    return line.a + direction * parameter;
}
