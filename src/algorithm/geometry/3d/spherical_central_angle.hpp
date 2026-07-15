#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"

namespace geometry3d_spherical_detail{

inline Point3 unit_direction(const Point3& point, const char* context){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument(context);
    }
    const long double scale = std::max({
        std::abs(point.x), std::abs(point.y), std::abs(point.z)
    });
    if(scale == 0.0L)[[unlikely]] throw std::invalid_argument(context);
    const Point3 scaled = point / scale;
    return scaled / std::hypot(scaled.x, scaled.y, scaled.z);
}

inline bool antipodal(const Point3& first, const Point3& second){
    const long double cosine = dot(first, second);
    const Point3 product = cross(first, second);
    const long double sine = std::hypot(
        product.x,
        product.y,
        product.z
    );
    return cosine < 0.0L
        && sine <= 64.0L * std::numeric_limits<long double>::epsilon();
}

inline void validate_radius(long double radius){
    if(!std::isfinite(radius) || radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument("invalid spherical radius");
    }
}

}  // namespace geometry3d_spherical_detail

inline long double spherical_central_angle(
    const Point3& first,
    const Point3& second
){
    const Point3 left = geometry3d_spherical_detail::unit_direction(
        first, "invalid first direction in spherical_central_angle"
    );
    const Point3 right = geometry3d_spherical_detail::unit_direction(
        second, "invalid second direction in spherical_central_angle"
    );
    const Point3 product = cross(left, right);
    return std::atan2(
        std::hypot(product.x, product.y, product.z),
        std::clamp(dot(left, right), -1.0L, 1.0L)
    );
}
