#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_SUM_3D_WITH_SEED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_SUM_3D_WITH_SEED_HPP_INCLUDED

#include <cstdint>
#include <stdexcept>
#include <utility>

#include "../point_set/convex_hull_3d_default.hpp"
#include "../point_set/convex_hull_3d_with_seed.hpp"
#include "../../detail/polyhedron_polyhedron/minkowski_sum_3d_detail.hpp"

inline ConvexPolyhedron3 minkowski_sum_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
){
    if(first.affine_dimension == 3 && second.affine_dimension == 3
       && first.vertices.size() >= 4 && second.vertices.size() >= 4
       && first.faces.size() >= 4 && second.faces.size() >= 4){
        try{
            return minkowski_sum_3d_detail::direct_sum(
                first, second, 1.0e-9L
            );
        }catch(const std::domain_error&){
            // Degenerate normal fans use the deterministic fast fallback.
        }
        auto candidates = minkowski_sum_3d_detail::candidate_points(
            first, second
        );
        if(auto quick =
            minkowski_sum_3d_detail::certified_small_candidate_hull(
                candidates
            )){
            return *quick;
        }
        return convex_hull_3d(std::move(candidates));
    }
    return convex_hull_3d_with_seed(
        minkowski_sum_3d_detail::candidate_points(first, second),
        random_seed
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_SUM_3D_WITH_SEED_HPP_INCLUDED
