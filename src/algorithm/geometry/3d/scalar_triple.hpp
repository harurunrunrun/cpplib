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
#include "dot.hpp"

inline long double scalar_triple(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return dot(first, cross(second, third));
}
