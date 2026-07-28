#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_DIHEDRAL_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_DIHEDRAL_ANGLE_HPP_INCLUDED

#include "../point_point/angle.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/plane/plane3_unit_normal.hpp"

inline long double dihedral_angle(
    const Plane3& first,
    const Plane3& second
){
    return angle(
        plane3_unit_normal(first),
        plane3_unit_normal(second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_DIHEDRAL_ANGLE_HPP_INCLUDED
