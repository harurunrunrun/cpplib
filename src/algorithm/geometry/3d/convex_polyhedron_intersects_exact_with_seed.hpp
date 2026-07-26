#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_EXACT_WITH_SEED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_EXACT_WITH_SEED_HPP_INCLUDED

#include <cstdint>
#include <random>
#include <vector>

#include "convex_polyhedron_intersects_detail.hpp"

inline bool convex_polyhedron_intersects_exact_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
){
    using namespace convex_polyhedron_intersects_detail;
    std::vector<ExactHalfspace3> halfspaces =
        combined_exact_halfspaces(first, second);
    std::mt19937_64 random_engine(random_seed);
    return randomized_exact_feasible(halfspaces, random_engine);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_EXACT_WITH_SEED_HPP_INCLUDED
