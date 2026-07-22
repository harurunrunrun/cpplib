#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_CAP_SURFACE_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_CAP_SURFACE_AREA_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"

inline long double sphere_cap_surface_area(
    const Sphere3& sphere,
    long double height
){
    geometry3d_validate(sphere);
    if(!std::isfinite(height) || height < 0.0L ||
        height / 2.0L > sphere.radius)[[unlikely]]{
        throw std::invalid_argument("invalid sphere cap height");
    }
    return geometry3d_detail::checked_nonnegative_product(
        {2.0L * GEOMETRY3D_PI, sphere.radius, height},
        "sphere cap surface area is not representable"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_CAP_SURFACE_AREA_HPP_INCLUDED
