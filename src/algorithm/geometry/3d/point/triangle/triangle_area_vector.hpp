#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_AREA_VECTOR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_AREA_VECTOR_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../point_point/cross.hpp"

inline Point3 triangle_area_vector(const Triangle3& triangle){
    return cross(triangle.b - triangle.a, triangle.c - triangle.a) / 2;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TRIANGLE_TRIANGLE_AREA_VECTOR_HPP_INCLUDED
