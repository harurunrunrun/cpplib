#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_DIFFERENCE_3D_WITH_SEED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_DIFFERENCE_3D_WITH_SEED_HPP_INCLUDED

#include <cstdint>

#include "../point_set/convex_hull_3d_with_seed.hpp"
#include "../../detail/polyhedron_polyhedron/minkowski_difference_3d_detail.hpp"

inline ConvexPolyhedron3 minkowski_difference_3d_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
){
    return convex_hull_3d_with_seed(
        minkowski_difference_3d_detail::candidate_points(first, second),
        random_seed
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_MINKOWSKI_DIFFERENCE_3D_WITH_SEED_HPP_INCLUDED
