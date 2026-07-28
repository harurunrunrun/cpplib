#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_VIA_HALFSPACES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_VIA_HALFSPACES_HPP_INCLUDED

#include <vector>

#include "../../detail/polyhedron/convex_polyhedron_intersection_detail.hpp"
#include "../../predicate/polyhedron_polyhedron/convex_polyhedron_intersects_exact.hpp"
#include "../../aggregate/all/halfspace_intersection_3d.hpp"

inline ConvexPolyhedron3 convex_polyhedron_intersection_via_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.vertices.empty() || second.vertices.empty()){
        return {-1, {}, {}};
    }
    const std::vector<Plane3> halfspaces =
        convex_polyhedron_intersection_detail::combined_halfspaces(
            first, second
        );
    if(!convex_polyhedron_intersects_exact(first, second)){
        return {-1, {}, {}};
    }
    return halfspace_intersection_3d(halfspaces);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_VIA_HALFSPACES_HPP_INCLUDED
