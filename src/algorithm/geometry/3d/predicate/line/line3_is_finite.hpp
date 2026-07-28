#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE3_IS_FINITE_HPP_INCLUDED

#include "../../type/definition/line3.hpp"
#include "../point/point3_is_finite.hpp"

inline bool geometry3d_is_finite(const Line3& line) noexcept{
    return geometry3d_is_finite(line.a) && geometry3d_is_finite(line.b);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE3_IS_FINITE_HPP_INCLUDED
