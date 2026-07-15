#pragma once

#include "base.hpp"
#include "is_finite.hpp"

inline long double sphere_surface_area(const Sphere3& sphere){
    geometry3d_validate(sphere);
    return geometry3d_detail::checked_nonnegative_product(
        {4.0L * GEOMETRY3D_PI, sphere.radius, sphere.radius},
        "sphere surface area is not representable"
    );
}
