#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_PARALLEL_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "../scalar/abs.hpp"
#include "../point/cross.hpp"
#include "is_finite.hpp"

inline bool parallel(const Point3& left, const Point3& right){
    if(!geometry3d_is_finite(left) || !geometry3d_is_finite(right))[[unlikely]]{
        throw std::invalid_argument("parallel requires finite vectors");
    }
    const long double left_scale = std::max({
        std::abs(left.x), std::abs(left.y), std::abs(left.z)
    });
    const long double right_scale = std::max({
        std::abs(right.x), std::abs(right.y), std::abs(right.z)
    });
    if(left_scale == 0.0L || right_scale == 0.0L) return true;
    const Point3 scaled_left = left / left_scale;
    const Point3 scaled_right = right / right_scale;
    return abs(cross(scaled_left, scaled_right)) <=
        GEOMETRY3D_EPS * abs(scaled_left) * abs(scaled_right);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_PARALLEL_HPP_INCLUDED
