#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_DIAMETER_AABB_BRANCH_AND_BOUND_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_DIAMETER_AABB_BRANCH_AND_BOUND_HPP_INCLUDED

#include <optional>

#include "../../type/definition/convex_polyhedron3.hpp"
#include "../../aggregate/all/point_set_diameter_3d.hpp"

inline std::optional<PointSetDiameter3DResult>
convex_polyhedron_diameter_aabb_branch_and_bound(
    const ConvexPolyhedron3& polyhedron
){
    return point_set_diameter_3d_aabb_branch_and_bound(
        polyhedron.vertices
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_CONVEX_POLYHEDRON_DIAMETER_AABB_BRANCH_AND_BOUND_HPP_INCLUDED
