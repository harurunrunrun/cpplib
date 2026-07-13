#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "parallel.hpp"

inline bool collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return parallel(second - first, third - first);
}
