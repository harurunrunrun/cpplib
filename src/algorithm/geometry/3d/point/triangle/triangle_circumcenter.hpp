#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_CIRCUMCENTER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_CIRCUMCENTER_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../circle/triangle/circumcircle.hpp"

inline Point3 triangle_circumcenter(const Triangle3& triangle){
    return circumcircle(triangle).center;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_CIRCUMCENTER_HPP_INCLUDED
