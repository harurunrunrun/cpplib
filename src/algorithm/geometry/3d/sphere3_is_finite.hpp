#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE3_IS_FINITE_HPP_INCLUDED

#include "point3_is_finite.hpp"
#include "sphere3.hpp"

inline bool geometry3d_is_finite(const Sphere3& sphere) noexcept{
    return geometry3d_is_finite(sphere.center) && geometry3d_is_finite(sphere.radius);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE3_IS_FINITE_HPP_INCLUDED
