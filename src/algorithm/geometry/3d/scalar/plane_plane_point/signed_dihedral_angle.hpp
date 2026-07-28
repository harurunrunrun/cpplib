#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_POINT_SIGNED_DIHEDRAL_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_POINT_SIGNED_DIHEDRAL_ANGLE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/point_point/cross.hpp"
#include "../point_point/dot.hpp"
#include "../../predicate/point_point/orthogonal.hpp"
#include "../../point/plane/plane3_unit_normal.hpp"
#include "../../point/point/unit.hpp"

inline long double signed_dihedral_angle(
    const Plane3& first,
    const Plane3& second,
    const Point3& oriented_axis
){
    const Point3 first_normal = plane3_unit_normal(first);
    const Point3 second_normal = plane3_unit_normal(second);
    const Point3 axis = unit(oriented_axis);
    if(!orthogonal(axis, first_normal) || !orthogonal(axis, second_normal)){
        throw std::invalid_argument("dihedral axis must lie in both planes");
    }
    return std::atan2(
        dot(axis, cross(first_normal, second_normal)),
        dot(first_normal, second_normal)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_PLANE_PLANE_POINT_SIGNED_DIHEDRAL_ANGLE_HPP_INCLUDED
