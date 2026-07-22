#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CLOSEST_POINT_HPP_INCLUDED

#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "abs.hpp"
#include "base.hpp"
#include "is_finite.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "plane3_unit_normal.hpp"

inline Point3 closest_point(const Circle3& circle, const Point3& point){
    geometry3d_validate(circle);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("circle closest point requires a finite point");
    }
    const Point3 normal = plane3_unit_normal({circle.center, circle.normal});
    if(circle.radius == 0.0L) return circle.center;
    const auto difference = geometry3d_normalized_difference(
        point, circle.center, {circle.radius}
    );
    const Point3 offset = difference.value;
    const Point3 radial = offset - normal * dot(offset, normal);
    const long double radial_length = std::hypot(radial.x, radial.y, radial.z);
    if(geometry3d_scaled_sign(
        radial_length, std::max(radial_length, circle.radius / difference.scale)
    ) == 0)[[unlikely]]{
        throw std::domain_error("closest point on circle is not unique");
    }
    const Point3 result = circle.center +
        radial * (circle.radius / radial_length);
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "closest point on circle is not representable"
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE_CLOSEST_POINT_HPP_INCLUDED
