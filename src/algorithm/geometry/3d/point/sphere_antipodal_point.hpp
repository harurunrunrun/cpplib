#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERE_ANTIPODAL_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERE_ANTIPODAL_POINT_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "../predicate/is_finite.hpp"
#include "../predicate/on_sphere.hpp"

inline Point3 sphere_antipodal_point(
    const Sphere3& sphere,
    const Point3& point
){
    if(!on_sphere(sphere, point))[[unlikely]]{
        throw std::invalid_argument("point is not on sphere");
    }
    const Geometry3DNormalizedDifference offset =
        geometry3d_normalized_difference(
            point, sphere.center, {sphere.radius}
        );
    const Point3 result{
        std::fma(-offset.value.x, offset.scale, sphere.center.x),
        std::fma(-offset.value.y, offset.scale, sphere.center.y),
        std::fma(-offset.value.z, offset.scale, sphere.center.z),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "sphere antipodal point is not representable"
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERE_ANTIPODAL_POINT_HPP_INCLUDED
