#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_HALFSPACE_INTERSECTION_3D_WITH_INTERIOR_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_HALFSPACE_INTERSECTION_3D_WITH_INTERIOR_POINT_HPP_INCLUDED

#include <vector>

#include "geometry_primitives.hpp"
#include "halfspace_intersection_3d_detail.hpp"

inline ConvexPolyhedron3 halfspace_intersection_3d_with_interior_point(
    const std::vector<Plane3>& halfspaces,
    const Point3& strict_interior_point
){
    using namespace halfspace_intersection_3d_detail;
    validate_halfspaces(halfspaces);
    return intersection_from_strict_interior(
        halfspaces, strict_interior_point
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_HALFSPACE_INTERSECTION_3D_WITH_INTERIOR_POINT_HPP_INCLUDED
