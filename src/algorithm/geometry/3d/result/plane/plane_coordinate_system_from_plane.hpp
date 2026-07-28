#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_PLANE_PLANE_COORDINATE_SYSTEM_FROM_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_PLANE_PLANE_COORDINATE_SYSTEM_FROM_PLANE_HPP_INCLUDED

#include "../../type/definition/plane_coordinate_system3.hpp"
#include "../../point/plane/plane3_unit_normal.hpp"
#include "../../type/definition/plane_orthonormal_basis.hpp"

inline PlaneCoordinateSystem3 plane_coordinate_system(const Plane3& plane){
    const Point3 normal = plane3_unit_normal(plane);
    const auto [first, second] = plane_orthonormal_basis(plane);
    return {plane.point, first, second, normal};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_PLANE_PLANE_COORDINATE_SYSTEM_FROM_PLANE_HPP_INCLUDED
