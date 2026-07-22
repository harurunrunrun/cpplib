#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_SPHERE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_SPHERE_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"
#include "geometry3d_sign.hpp"
#include "dot.hpp"
#include "norm.hpp"
#include "plane3_unit_normal.hpp"
#include "projection.hpp"

inline std::optional<Circle3> plane_sphere_intersection(
    const Plane3& plane,
    const Sphere3& sphere
){
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument("plane-sphere intersection requires a finite plane");
    }
    const Point3 normal = plane3_unit_normal(plane);
    const auto difference = geometry3d_normalized_difference(
        sphere.center, plane.point, {sphere.radius}
    );
    const long double scale = difference.scale;
    const long double radius = sphere.radius / scale;
    const Point3 center_offset = difference.value;
    const long double signed_height = dot(center_offset, normal);
    const long double height = std::abs(signed_height);
    const long double squared_radius = (radius - height) * (radius + height);
    if(geometry3d_scaled_sign(
        squared_radius, std::max(radius * radius, height * height)
    ) < 0) return std::nullopt;
    const long double normalized_radius = std::sqrt(
        std::max(0.0L, squared_radius)
    );
    const Point3 center = sphere.center - normal * (signed_height * scale);
    const long double circle_radius = normalized_radius * scale;
    if(!geometry3d_is_finite(center) || !std::isfinite(circle_radius))
        [[unlikely]]{
        throw std::overflow_error(
            "plane-sphere intersection circle is not representable"
        );
    }
    return Circle3{center, normal, circle_radius};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_SPHERE_INTERSECTION_HPP_INCLUDED
