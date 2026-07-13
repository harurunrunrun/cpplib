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
#include "on_plane.hpp"

inline bool on_circle(const Circle3& circle, const Point3& point){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    return on_plane({circle.center, circle.normal}, point) &&
        geometry3d_sign(abs(point - circle.center) - circle.radius) == 0;
}
