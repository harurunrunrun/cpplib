#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_CIRCUMCENTER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_CIRCUMCENTER_HPP_INCLUDED

#include "../core/geometry_primitives.hpp"
#include "../shape/circumcircle.hpp"

inline Point3 triangle_circumcenter(const Triangle3& triangle){
    return circumcircle(triangle).center;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_CIRCUMCENTER_HPP_INCLUDED
