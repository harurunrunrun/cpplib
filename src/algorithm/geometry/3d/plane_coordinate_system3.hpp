#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_COORDINATE_SYSTEM3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_COORDINATE_SYSTEM3_HPP_INCLUDED

#include "geometry_primitives.hpp"

struct PlaneCoordinateSystem3{
    Point3 origin;
    Point3 first_axis;
    Point3 second_axis;
    Point3 normal;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PLANE_COORDINATE_SYSTEM3_HPP_INCLUDED
