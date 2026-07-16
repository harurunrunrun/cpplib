#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <utility>

#include "base.hpp"

inline bool geometry3d_is_finite(long double value) noexcept{
    return std::isfinite(value);
}

inline bool geometry3d_is_finite(const Point3& point) noexcept{
    return geometry3d_is_finite(point.x) && geometry3d_is_finite(point.y)
        && geometry3d_is_finite(point.z);
}

inline bool geometry3d_is_finite(const Line3& line) noexcept{
    return geometry3d_is_finite(line.a) && geometry3d_is_finite(line.b);
}

inline bool geometry3d_is_finite(const Ray3& ray) noexcept{
    return geometry3d_is_finite(ray.origin) && geometry3d_is_finite(ray.through);
}

inline bool geometry3d_is_finite(const Segment3& segment) noexcept{
    return geometry3d_is_finite(segment.a) && geometry3d_is_finite(segment.b);
}

inline bool geometry3d_is_finite(const Plane3& plane) noexcept{
    return geometry3d_is_finite(plane.point) && geometry3d_is_finite(plane.normal);
}

inline bool geometry3d_is_finite(const Circle3& circle) noexcept{
    return geometry3d_is_finite(circle.center) && geometry3d_is_finite(circle.normal)
        && geometry3d_is_finite(circle.radius);
}

inline bool geometry3d_is_finite(const Sphere3& sphere) noexcept{
    return geometry3d_is_finite(sphere.center) && geometry3d_is_finite(sphere.radius);
}

inline bool geometry3d_is_finite(const Triangle3& triangle) noexcept{
    return geometry3d_is_finite(triangle.a) && geometry3d_is_finite(triangle.b)
        && geometry3d_is_finite(triangle.c);
}

inline bool geometry3d_is_finite(const Tetrahedron3& tetrahedron) noexcept{
    return geometry3d_is_finite(tetrahedron.a)
        && geometry3d_is_finite(tetrahedron.b)
        && geometry3d_is_finite(tetrahedron.c)
        && geometry3d_is_finite(tetrahedron.d);
}

inline long double geometry3d_safe_normalized_difference(
    long double left,
    long double right,
    long double positive_scale
){
    if(!std::isfinite(left) || !std::isfinite(right) ||
        !std::isfinite(positive_scale) || positive_scale <= 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "normalized difference requires finite values and a positive scale"
        );
    }
    const long double result =
        std::signbit(left) == std::signbit(right)
        ? (left - right) / positive_scale
        : left / positive_scale - right / positive_scale;
    if(result == 0.0L && left != right)[[unlikely]]{
        throw std::overflow_error(
            "normalized difference is not representable"
        );
    }
    return result;
}

inline Point3 geometry3d_safe_normalized_difference(
    const Point3& left,
    const Point3& right,
    long double positive_scale
){
    return {
        geometry3d_safe_normalized_difference(left.x, right.x, positive_scale),
        geometry3d_safe_normalized_difference(left.y, right.y, positive_scale),
        geometry3d_safe_normalized_difference(left.z, right.z, positive_scale),
    };
}

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

inline int geometry3d_scaled_sign(
    long double value,
    long double nonnegative_characteristic_scale
){
    if(!std::isfinite(value) ||
        !std::isfinite(nonnegative_characteristic_scale) ||
        nonnegative_characteristic_scale < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "scaled sign requires finite value and nonnegative scale"
        );
    }
    const long double tolerance =
        GEOMETRY3D_EPS * nonnegative_characteristic_scale;
    if(value > tolerance) return 1;
    if(value < -tolerance) return -1;
    return 0;
}

inline int geometry3d_compare_nonnegative_scaled(
    long double left_value,
    long double left_scale,
    long double right_value,
    long double right_scale
){
    if(!std::isfinite(left_value) || !std::isfinite(left_scale) ||
        !std::isfinite(right_value) || !std::isfinite(right_scale) ||
        left_value < 0.0L || right_value < 0.0L ||
        left_scale <= 0.0L || right_scale <= 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "scaled comparison requires finite nonnegative values and positive scales"
        );
    }
    if(left_value == 0.0L || right_value == 0.0L){
        return (left_value > 0.0L) - (right_value > 0.0L);
    }
    const auto representation = [](long double value, long double scale){
        int value_exponent = 0;
        int scale_exponent = 0;
        long double mantissa = std::frexp(value, &value_exponent) *
            std::frexp(scale, &scale_exponent);
        int exponent = value_exponent + scale_exponent;
        if(mantissa < 0.5L){
            mantissa *= 2.0L;
            --exponent;
        }
        return std::pair<long double, int>{mantissa, exponent};
    };
    const auto left = representation(left_value, left_scale);
    const auto right = representation(right_value, right_scale);
    if(left.second != right.second){
        return left.second < right.second ? -1 : 1;
    }
    return (left.first > right.first) - (left.first < right.first);
}

namespace geometry3d_detail{

inline long double checked_nonnegative_product(
    std::initializer_list<long double> factors,
    const char* overflow_message
){
    long double mantissa = 1.0L;
    long long exponent = 0;
    for(const long double factor: factors){
        if(!std::isfinite(factor) || factor < 0.0L)[[unlikely]]{
            throw std::invalid_argument(
                "product requires finite nonnegative factors"
            );
        }
        if(factor == 0.0L) return 0.0L;
        int factor_exponent = 0;
        const long double factor_mantissa =
            std::frexp(factor, &factor_exponent);
        mantissa *= factor_mantissa;
        exponent += factor_exponent;
        if(mantissa < 0.5L){
            mantissa *= 2.0L;
            --exponent;
        }
    }
    if(exponent > std::numeric_limits<long double>::max_exponent)
        [[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    const long double result = std::ldexp(
        mantissa, static_cast<int>(exponent)
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    return result;
}

}

inline void geometry3d_validate(const Circle3& circle){
    if(!geometry3d_is_finite(circle))[[unlikely]]{
        throw std::invalid_argument("circle requires finite center, normal, and radius");
    }
    if(circle.radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    if(circle.normal.x == 0.0L && circle.normal.y == 0.0L
        && circle.normal.z == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero circle normal");
    }
}

inline void geometry3d_validate(const Sphere3& sphere){
    if(!geometry3d_is_finite(sphere))[[unlikely]]{
        throw std::invalid_argument("sphere requires finite center and radius");
    }
    if(sphere.radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
}
