#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_SPHERE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_SPHERE_HPP_INCLUDED

#include <stdexcept>

#include "../scalar/abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "is_finite.hpp"
#include "geometry3d_sign.hpp"

inline bool on_sphere(const Sphere3& sphere, const Point3& point){
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("on_sphere requires a finite point");
    }
    const auto difference = geometry3d_normalized_difference(
        point, sphere.center, {sphere.radius}
    );
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double radius = sphere.radius / difference.scale;
    return geometry3d_scaled_sign(
        center_distance - radius, std::max(center_distance, radius)
    ) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_SPHERE_HPP_INCLUDED
