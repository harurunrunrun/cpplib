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
#include "plane3_unit_normal.hpp"

inline bool on_plane(const Plane3& plane, const Point3& point){
    const Point3 normal = plane3_unit_normal(plane);
    return geometry3d_sign(dot(normal, point - plane.point)) == 0;
}
