#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "projection.hpp"

inline Point3 reflection(const Line3& line, const Point3& point){
    return point + (projection(line, point) - point) * 2;
}

inline Point3 reflection(const Plane3& plane, const Point3& point){
    return point + (projection(plane, point) - point) * 2;
}
