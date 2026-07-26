#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT3_IS_FINITE_HPP_INCLUDED

#include "point3_is_finite.hpp"
#include "segment3.hpp"

inline bool geometry3d_is_finite(const Segment3& segment) noexcept{
    return geometry3d_is_finite(segment.a) && geometry3d_is_finite(segment.b);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT3_IS_FINITE_HPP_INCLUDED
