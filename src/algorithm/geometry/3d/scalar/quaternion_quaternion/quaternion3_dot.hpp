#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_QUATERNION_QUATERNION_QUATERNION3_DOT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_QUATERNION_QUATERNION_QUATERNION3_DOT_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../../type/definition/quaternion3_type.hpp"

inline long double quaternion3_dot(
    const Quaternion3& first,
    const Quaternion3& second
){
    if(!first.is_finite() || !second.is_finite())[[unlikely]]{
        throw std::invalid_argument("non-finite quaternion dot product");
    }
    const long double result = first.w * second.w + first.x * second.x
        + first.y * second.y + first.z * second.z;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("quaternion dot product overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_QUATERNION_QUATERNION_QUATERNION3_DOT_HPP_INCLUDED
