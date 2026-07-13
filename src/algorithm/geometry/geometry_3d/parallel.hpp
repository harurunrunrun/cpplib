#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "cross.hpp"
#include "geometry3d_sign.hpp"

inline bool parallel(const Point3& left, const Point3& right){
    return geometry3d_sign(abs(cross(left, right))) == 0;
}
