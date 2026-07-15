#pragma once

#include <stdexcept>

#include "base.hpp"
#include "cross.hpp"
#include "is_finite.hpp"
#include "quaternion3.hpp"

inline Point3 quaternion3_rotate(
    const Quaternion3& quaternion,
    const Point3& point
){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("non-finite quaternion rotation point");
    }
    const Quaternion3 rotation = quaternion.normalized();
    const Point3 vector{rotation.x, rotation.y, rotation.z};
    const Point3 twice_cross = 2.0L * cross(vector, point);
    const Point3 result = point + rotation.w * twice_cross
        + cross(vector, twice_cross);
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error("quaternion vector rotation overflow");
    }
    return result;
}
