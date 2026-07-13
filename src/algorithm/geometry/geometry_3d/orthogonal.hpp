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

inline bool orthogonal(const Point3& left, const Point3& right){
    return geometry3d_sign(dot(left, right)) == 0;
}
