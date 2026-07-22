#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERE_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERE_POINT_DISTANCE_HPP_INCLUDED

#include <cmath>
#include <algorithm>
#include <limits>
#include <stdexcept>

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../predicate/is_finite.hpp"

inline long double distance(const Sphere3& sphere, const Point3& point){
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("sphere-point distance requires a finite point");
    }
    const auto difference = geometry3d_normalized_difference(
        point, sphere.center, {sphere.radius}
    );
    const long double normalized_distance = std::abs(
        std::hypot(
            difference.value.x, difference.value.y, difference.value.z
        ) - sphere.radius / difference.scale
    );
    if(normalized_distance > std::numeric_limits<long double>::max() / difference.scale)
        [[unlikely]]{
        throw std::overflow_error(
            "sphere-point distance is not representable"
        );
    }
    return normalized_distance * difference.scale;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERE_POINT_DISTANCE_HPP_INCLUDED
