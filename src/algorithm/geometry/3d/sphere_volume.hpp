#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_VOLUME_HPP_INCLUDED

#include "base.hpp"
#include "is_finite.hpp"

inline long double sphere_volume(const Sphere3& sphere){
    geometry3d_validate(sphere);
    return geometry3d_detail::checked_nonnegative_product(
        {
            4.0L * GEOMETRY3D_PI / 3.0L,
            sphere.radius,
            sphere.radius,
            sphere.radius,
        },
        "sphere volume is not representable"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE_VOLUME_HPP_INCLUDED
