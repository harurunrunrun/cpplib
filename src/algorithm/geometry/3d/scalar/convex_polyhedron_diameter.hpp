#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_DIAMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_DIAMETER_HPP_INCLUDED

#include <optional>

#include "../core/convex_polyhedron3.hpp"
#include "point_set_diameter_3d.hpp"

inline std::optional<PointSetDiameter3DResult> convex_polyhedron_diameter(
    const ConvexPolyhedron3& polyhedron
){
    return point_set_diameter_3d(polyhedron.vertices);
}

inline std::optional<PointSetDiameter3DResult>
convex_polyhedron_diameter_aabb_branch_and_bound(
    const ConvexPolyhedron3& polyhedron
){
    return point_set_diameter_3d_aabb_branch_and_bound(
        polyhedron.vertices
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_DIAMETER_HPP_INCLUDED
