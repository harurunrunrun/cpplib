#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY3_IS_FINITE_HPP_INCLUDED

#include "point3_is_finite.hpp"
#include "ray3.hpp"

inline bool geometry3d_is_finite(const Ray3& ray) noexcept{
    return geometry3d_is_finite(ray.origin) && geometry3d_is_finite(ray.through);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY3_IS_FINITE_HPP_INCLUDED
