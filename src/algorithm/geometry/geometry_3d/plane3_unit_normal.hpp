#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "unit.hpp"

inline Point3 plane3_unit_normal(const Plane3& plane){
    return unit(plane.normal);
}
