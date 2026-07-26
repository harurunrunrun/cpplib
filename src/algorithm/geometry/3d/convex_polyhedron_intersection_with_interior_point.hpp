#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_WITH_INTERIOR_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_WITH_INTERIOR_POINT_HPP_INCLUDED

#include "convex_polyhedron_intersection_detail.hpp"
#include "halfspace_intersection_3d.hpp"

inline ConvexPolyhedron3
convex_polyhedron_intersection_via_halfspaces_with_interior_point(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const Point3& strict_interior_point
){
    return halfspace_intersection_3d_with_interior_point(
        convex_polyhedron_intersection_detail::combined_halfspaces(
            first, second
        ),
        strict_interior_point
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_WITH_INTERIOR_POINT_HPP_INCLUDED
