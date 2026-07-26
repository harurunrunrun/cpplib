#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE3_VALIDATE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE3_VALIDATE_HPP_INCLUDED

#include <stdexcept>

#include "sphere3_is_finite.hpp"

inline void geometry3d_validate(const Sphere3& sphere){
    if(!geometry3d_is_finite(sphere))[[unlikely]]{
        throw std::invalid_argument("sphere requires finite center and radius");
    }
    if(sphere.radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SPHERE3_VALIDATE_HPP_INCLUDED
