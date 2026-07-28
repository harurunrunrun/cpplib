#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INTEGER_SPHERE_POINT_SPHERE_CONTAINS_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INTEGER_SPHERE_POINT_SPHERE_CONTAINS_POINT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../scalar/point/abs.hpp"
#include "../scalar/geometry3d_sign.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/sphere3.hpp"

inline int contains(const Sphere3& sphere, const Point3& point){
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("sphere containment requires a finite point");
    }
    const auto difference = geometry3d_normalized_difference(
        point, sphere.center, {sphere.radius}
    );
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double radius = sphere.radius / difference.scale;
    return -geometry3d_scaled_sign(
        center_distance - radius, std::max(center_distance, radius)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INTEGER_SPHERE_POINT_SPHERE_CONTAINS_POINT_HPP_INCLUDED
