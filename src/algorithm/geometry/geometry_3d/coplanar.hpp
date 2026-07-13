#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "scalar_triple.hpp"

inline bool coplanar(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    return geometry3d_sign(scalar_triple(
        second - first, third - first, fourth - first
    )) == 0;
}
