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
#include "line3_direction.hpp"
#include "norm.hpp"
#include "plane3_unit_normal.hpp"

inline Point3 projection(const Line3& line, const Point3& point){
    const Point3 direction = line3_direction(line);
    return line.a + direction * (dot(point - line.a, direction) / norm(direction));
}

inline Point3 projection(const Plane3& plane, const Point3& point){
    const Point3 normal = plane3_unit_normal(plane);
    return point - normal * dot(point - plane.point, normal);
}
