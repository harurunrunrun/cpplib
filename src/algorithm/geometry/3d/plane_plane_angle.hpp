#pragma once

#include <algorithm>
#include <cmath>

#include "base.hpp"
#include "dot.hpp"
#include "plane3_unit_normal.hpp"

inline long double plane_plane_angle(
    const Plane3& first,
    const Plane3& second
){
    return std::acos(std::clamp(
        std::abs(dot(
            plane3_unit_normal(first), plane3_unit_normal(second)
        )),
        0.0L,
        1.0L
    ));
}
