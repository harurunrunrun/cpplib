#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RELATIVE_EQUAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RELATIVE_EQUAL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"

inline bool relative_equal(
    long double first,
    long double second,
    long double relative_tolerance = GEOMETRY3D_EPS,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second)
        || !geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || relative_tolerance < 0 || absolute_tolerance < 0)[[unlikely]]{
        throw std::invalid_argument("non-finite value or invalid 3D comparison tolerance");
    }
    if(first == second) return true;
    const long double difference = std::abs(first - second);
    const long double scale = std::max(std::abs(first), std::abs(second));
    const long double relative_bound = relative_tolerance * scale;
    if(!geometry3d_is_finite(relative_bound)) return true;
    return difference <= std::max(absolute_tolerance, relative_bound);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RELATIVE_EQUAL_HPP_INCLUDED
