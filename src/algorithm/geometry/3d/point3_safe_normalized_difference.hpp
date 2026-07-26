#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT3_SAFE_NORMALIZED_DIFFERENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT3_SAFE_NORMALIZED_DIFFERENCE_HPP_INCLUDED

#include "long_double_safe_normalized_difference.hpp"
#include "point3.hpp"

inline Point3 geometry3d_safe_normalized_difference(
    const Point3& left,
    const Point3& right,
    long double positive_scale
){
    return {
        geometry3d_safe_normalized_difference(left.x, right.x, positive_scale),
        geometry3d_safe_normalized_difference(left.y, right.y, positive_scale),
        geometry3d_safe_normalized_difference(left.z, right.z, positive_scale),
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT3_SAFE_NORMALIZED_DIFFERENCE_HPP_INCLUDED
