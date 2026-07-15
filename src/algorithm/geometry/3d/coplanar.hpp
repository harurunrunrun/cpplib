#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "adaptive_orient3d.hpp"

inline bool coplanar(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    return adaptive_orient3d(first, second, third, fourth) == 0;
}
