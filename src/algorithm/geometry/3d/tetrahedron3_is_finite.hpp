#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON3_IS_FINITE_HPP_INCLUDED

#include "point3_is_finite.hpp"
#include "tetrahedron3.hpp"

inline bool geometry3d_is_finite(const Tetrahedron3& tetrahedron) noexcept{
    return geometry3d_is_finite(tetrahedron.a)
        && geometry3d_is_finite(tetrahedron.b)
        && geometry3d_is_finite(tetrahedron.c)
        && geometry3d_is_finite(tetrahedron.d);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TETRAHEDRON3_IS_FINITE_HPP_INCLUDED
