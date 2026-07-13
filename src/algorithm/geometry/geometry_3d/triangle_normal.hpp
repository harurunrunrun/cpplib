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
#include "unit.hpp"

inline Point3 triangle_normal(const Triangle3& triangle){
    return unit(cross(triangle.b - triangle.a, triangle.c - triangle.a));
}
