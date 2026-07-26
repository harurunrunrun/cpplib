#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT3_NORMALIZED_DIFFERENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT3_NORMALIZED_DIFFERENCE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <stdexcept>

#include "point3_is_finite.hpp"
#include "point3_safe_normalized_difference.hpp"

struct Geometry3DNormalizedDifference{
    Point3 value;
    long double scale;
};

inline Geometry3DNormalizedDifference geometry3d_normalized_difference(
    const Point3& left,
    const Point3& right,
    std::initializer_list<long double> additional_extents = {}
){
    if(!geometry3d_is_finite(left) || !geometry3d_is_finite(right))
        [[unlikely]]{
        throw std::invalid_argument(
            "normalized point difference requires finite points"
        );
    }
    long double scale = 0.0L;
    for(const long double extent: additional_extents){
        if(!std::isfinite(extent) || extent < 0.0L)[[unlikely]]{
            throw std::invalid_argument(
                "normalized point difference requires finite nonnegative extents"
            );
        }
        scale = std::max(scale, extent);
    }

    const std::array<long double, 3> left_values{left.x, left.y, left.z};
    const std::array<long double, 3> right_values{right.x, right.y, right.z};
    std::array<long double, 3> raw{};
    std::array<bool, 3> raw_is_safe{};
    const long double maximum = std::numeric_limits<long double>::max();
    for(std::size_t index = 0; index < 3; ++index){
        const long double left_value = left_values[index];
        const long double right_value = right_values[index];
        const long double left_abs = std::abs(left_value);
        const long double right_abs = std::abs(right_value);
        const bool same_sign =
            std::signbit(left_value) == std::signbit(right_value);
        const bool safe_opposite_sign = same_sign ||
            left_abs <= maximum - right_abs;
        raw_is_safe[index] = safe_opposite_sign;
        if(safe_opposite_sign){
            raw[index] = left_value - right_value;
            scale = std::max(scale, std::abs(raw[index]));
        }else{
            scale = std::max({scale, left_abs, right_abs});
        }
    }
    if(scale == 0.0L) return {{0, 0, 0}, 1.0L};

    Point3 normalized;
    long double* const coordinates[] = {
        &normalized.x, &normalized.y, &normalized.z,
    };
    for(std::size_t index = 0; index < 3; ++index){
        if(raw_is_safe[index]){
            *coordinates[index] = raw[index] / scale;
            if(*coordinates[index] == 0.0L && raw[index] != 0.0L)
                [[unlikely]]{
                throw std::overflow_error(
                    "normalized point difference is not representable"
                );
            }
        }else{
            *coordinates[index] = geometry3d_safe_normalized_difference(
                left_values[index], right_values[index], scale
            );
        }
    }
    return {normalized, scale};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT3_NORMALIZED_DIFFERENCE_HPP_INCLUDED
