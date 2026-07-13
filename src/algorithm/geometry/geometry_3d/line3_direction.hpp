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

inline Point3 line3_direction(const Line3& line){
    const Point3 direction = line.b - line.a;
    if(geometry3d_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D line");
    }
    return direction;
}
