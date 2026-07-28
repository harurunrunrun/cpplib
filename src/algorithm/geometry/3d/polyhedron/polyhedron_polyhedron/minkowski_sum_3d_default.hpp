#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_SUM_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_SUM_3D_DEFAULT_HPP_INCLUDED

#include <stdexcept>

#include "../point_set/convex_hull_3d_default.hpp"
#include "../../detail/polyhedron_polyhedron/minkowski_sum_3d_detail.hpp"

inline ConvexPolyhedron3 minkowski_sum_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.affine_dimension == 3 && second.affine_dimension == 3
       && first.vertices.size() >= 4 && second.vertices.size() >= 4
       && first.faces.size() >= 4 && second.faces.size() >= 4){
        try{
            return minkowski_sum_3d_detail::direct_sum(
                first, second, 1.0e-9L
            );
        }catch(const std::domain_error&){
            // Degenerate normal fans are handled by the general fallback.
        }
    }
    return convex_hull_3d(
        minkowski_sum_3d_detail::candidate_points(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_SUM_3D_DEFAULT_HPP_INCLUDED
