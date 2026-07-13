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

inline Point3 ray3_direction(const Ray3& ray){
    const Point3 direction = ray.through - ray.origin;
    if(geometry3d_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D ray");
    }
    return direction;
}
