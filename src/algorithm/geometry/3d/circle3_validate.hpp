#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE3_VALIDATE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE3_VALIDATE_HPP_INCLUDED

#include <stdexcept>

#include "circle3_is_finite.hpp"

inline void geometry3d_validate(const Circle3& circle){
    if(!geometry3d_is_finite(circle))[[unlikely]]{
        throw std::invalid_argument(
            "circle requires finite center, normal, and radius"
        );
    }
    if(circle.radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    if(circle.normal.x == 0.0L && circle.normal.y == 0.0L
        && circle.normal.z == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero circle normal");
    }
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CIRCLE3_VALIDATE_HPP_INCLUDED
