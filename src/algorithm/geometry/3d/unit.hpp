#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_UNIT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_UNIT_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"

inline Point3 unit(const Point3& point){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("unit vector requires finite coordinates");
    }
    const long double scale = std::max({
        std::abs(point.x), std::abs(point.y), std::abs(point.z)
    });
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("unit vector of zero 3D vector");
    }
    const Point3 scaled = point / scale;
    const long double length = std::hypot(scaled.x, scaled.y, scaled.z);
    return scaled / length;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_UNIT_HPP_INCLUDED
