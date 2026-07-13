#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

inline long double dot(const Point3& left, const Point3& right){
    return left.x * right.x + left.y * right.y + left.z * right.z;
}
