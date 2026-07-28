#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT3_IS_FINITE_HPP_INCLUDED

#include "../scalar/long_double_is_finite.hpp"
#include "../../type/definition/point3.hpp"

inline bool geometry3d_is_finite(const Point3& point) noexcept{
    return geometry3d_is_finite(point.x) && geometry3d_is_finite(point.y)
        && geometry3d_is_finite(point.z);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POINT_POINT3_IS_FINITE_HPP_INCLUDED
