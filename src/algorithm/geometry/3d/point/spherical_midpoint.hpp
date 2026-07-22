#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERICAL_MIDPOINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERICAL_MIDPOINT_HPP_INCLUDED

#include <stdexcept>

#include "../scalar/spherical_central_angle.hpp"

inline Point3 spherical_midpoint(const Point3& first, const Point3& second){
    const Point3 left = geometry3d_spherical_detail::unit_direction(
        first, "invalid first direction in spherical_midpoint"
    );
    const Point3 right = geometry3d_spherical_detail::unit_direction(
        second, "invalid second direction in spherical_midpoint"
    );
    if(geometry3d_spherical_detail::antipodal(left, right))[[unlikely]]{
        throw std::domain_error("antipodal spherical midpoint is not unique");
    }
    return geometry3d_spherical_detail::unit_direction(
        left + right, "degenerate spherical midpoint"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SPHERICAL_MIDPOINT_HPP_INCLUDED
