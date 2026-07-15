#pragma once

#include <cmath>
#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "geometry3d_sign.hpp"

enum SphereRelation{
    SPHERE_INSIDE = 0,
    SPHERE_INTERNALLY_TANGENT = 1,
    SPHERE_INTERSECT = 2,
    SPHERE_EXTERNALLY_TANGENT = 3,
    SPHERE_SEPARATE = 4,
    SPHERE_COINCIDENT = 5,
};

inline SphereRelation sphere_relation(
    const Sphere3& first,
    const Sphere3& second
){
    if(first.radius < 0 || second.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const long double center_distance = abs(second.center - first.center);
    const long double radius_sum = first.radius + second.radius;
    const long double radius_difference = std::abs(first.radius - second.radius);
    if(geometry3d_sign(center_distance) == 0 &&
        geometry3d_sign(radius_difference) == 0){
        return SPHERE_COINCIDENT;
    }
    const int sum_comparison = geometry3d_sign(center_distance - radius_sum);
    if(sum_comparison > 0) return SPHERE_SEPARATE;
    if(sum_comparison == 0) return SPHERE_EXTERNALLY_TANGENT;
    const int difference_comparison =
        geometry3d_sign(center_distance - radius_difference);
    if(difference_comparison < 0) return SPHERE_INSIDE;
    if(difference_comparison == 0) return SPHERE_INTERNALLY_TANGENT;
    return SPHERE_INTERSECT;
}
