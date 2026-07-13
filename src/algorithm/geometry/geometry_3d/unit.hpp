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
#include "geometry3d_sign.hpp"

inline Point3 unit(const Point3& point){
    const long double length = abs(point);
    if(geometry3d_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("unit vector of zero 3D vector");
    }
    return point / length;
}
