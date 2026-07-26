#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE3_IS_FINITE_HPP_INCLUDED

#include "point3_is_finite.hpp"
#include "triangle3.hpp"

inline bool geometry3d_is_finite(const Triangle3& triangle) noexcept{
    return geometry3d_is_finite(triangle.a) && geometry3d_is_finite(triangle.b)
        && geometry3d_is_finite(triangle.c);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE3_IS_FINITE_HPP_INCLUDED
