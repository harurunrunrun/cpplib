#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "../point/plane3_unit_normal.hpp"
#include "../point_collection/plane_orthonormal_basis.hpp"

struct PlaneCoordinateSystem3{
    Point3 origin;
    Point3 first_axis;
    Point3 second_axis;
    Point3 normal;
};

inline PlaneCoordinateSystem3 plane_coordinate_system(const Plane3& plane){
    const Point3 normal = plane3_unit_normal(plane);
    const auto [first, second] = plane_orthonormal_basis(plane);
    return {plane.point, first, second, normal};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CORE_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED
