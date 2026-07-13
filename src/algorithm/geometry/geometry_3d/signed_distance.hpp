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
#include "plane3_unit_normal.hpp"

inline long double signed_distance(const Plane3& plane, const Point3& point){
    return dot(point - plane.point, plane3_unit_normal(plane));
}
