#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ABS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ABS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"

inline long double abs(const Point3& point){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("3D vector length requires finite coordinates");
    }
    const long double result = std::hypot(point.x, point.y, point.z);
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error(
            "3D vector length is not representable as long double"
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ABS_HPP_INCLUDED
