#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE3_IS_FINITE_HPP_INCLUDED

#include "plane3.hpp"
#include "point3_is_finite.hpp"

inline bool geometry3d_is_finite(const Plane3& plane) noexcept{
    return geometry3d_is_finite(plane.point) && geometry3d_is_finite(plane.normal);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE3_IS_FINITE_HPP_INCLUDED
