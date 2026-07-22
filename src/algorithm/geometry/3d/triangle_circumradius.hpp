#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_CIRCUMRADIUS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_CIRCUMRADIUS_HPP_INCLUDED

#include "base.hpp"
#include "circumcircle.hpp"

inline long double triangle_circumradius(const Triangle3& triangle){
    return circumcircle(triangle).radius;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_CIRCUMRADIUS_HPP_INCLUDED
