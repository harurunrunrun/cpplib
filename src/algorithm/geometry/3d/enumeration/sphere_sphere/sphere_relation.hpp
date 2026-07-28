#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ENUMERATION_SPHERE_SPHERE_SPHERE_RELATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ENUMERATION_SPHERE_SPHERE_SPHERE_RELATION_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../../scalar/point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"
#include "../../type/definition/sphere_relation_result.hpp"

inline SphereRelation sphere_relation(
    const Sphere3& first,
    const Sphere3& second
){
    geometry3d_validate(first);
    geometry3d_validate(second);
    const auto difference = geometry3d_normalized_difference(
        second.center, first.center, {first.radius, second.radius}
    );
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double first_radius = first.radius / difference.scale;
    const long double second_radius = second.radius / difference.scale;
    const long double radius_sum = first_radius + second_radius;
    const long double radius_difference = std::abs(first_radius - second_radius);
    const long double linear_scale = std::max({
        center_distance, first_radius, second_radius,
    });
    if(geometry3d_scaled_sign(center_distance, linear_scale) == 0 &&
        geometry3d_scaled_sign(radius_difference, linear_scale) == 0){
        return SPHERE_COINCIDENT;
    }
    const int sum_comparison = geometry3d_scaled_sign(
        center_distance - radius_sum, linear_scale
    );
    if(sum_comparison > 0) return SPHERE_SEPARATE;
    if(sum_comparison == 0) return SPHERE_EXTERNALLY_TANGENT;
    const int difference_comparison =
        geometry3d_scaled_sign(
            center_distance - radius_difference, linear_scale);
    if(difference_comparison < 0) return SPHERE_INSIDE;
    if(difference_comparison == 0) return SPHERE_INTERNALLY_TANGENT;
    return SPHERE_INTERSECT;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ENUMERATION_SPHERE_SPHERE_SPHERE_RELATION_HPP_INCLUDED
