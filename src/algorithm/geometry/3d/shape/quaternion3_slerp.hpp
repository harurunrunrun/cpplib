#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_QUATERNION3_SLERP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_QUATERNION3_SLERP_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "../core/quaternion3.hpp"

inline Quaternion3 quaternion3_slerp(
    const Quaternion3& first,
    const Quaternion3& second,
    long double parameter
){
    if(!std::isfinite(parameter) || parameter < 0.0L || parameter > 1.0L)[[unlikely]]{
        throw std::invalid_argument("quaternion slerp parameter outside [0,1]");
    }
    const Quaternion3 start = first.normalized();
    Quaternion3 finish = second.normalized();
    long double cosine = quaternion3_dot(start, finish);
    if(cosine < 0.0L){
        finish = -finish;
        cosine = -cosine;
    }
    cosine = std::clamp(cosine, -1.0L, 1.0L);
    if(cosine > 1.0L - 32.0L * std::numeric_limits<long double>::epsilon()){
        return ((1.0L - parameter) * start + parameter * finish).normalized();
    }
    const long double angle = std::acos(cosine);
    const long double sine = std::sin(angle);
    const long double first_weight = std::sin((1.0L - parameter) * angle) / sine;
    const long double second_weight = std::sin(parameter * angle) / sine;
    return (first_weight * start + second_weight * finish).normalized();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_QUATERNION3_SLERP_HPP_INCLUDED
