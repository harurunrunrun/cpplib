#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CIRCLE_CIRCLE3_IS_FINITE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CIRCLE_CIRCLE3_IS_FINITE_HPP_INCLUDED

#include "../../type/definition/circle3.hpp"
#include "../point/point3_is_finite.hpp"

inline bool geometry3d_is_finite(const Circle3& circle) noexcept{
    return geometry3d_is_finite(circle.center) && geometry3d_is_finite(circle.normal)
        && geometry3d_is_finite(circle.radius);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CIRCLE_CIRCLE3_IS_FINITE_HPP_INCLUDED
