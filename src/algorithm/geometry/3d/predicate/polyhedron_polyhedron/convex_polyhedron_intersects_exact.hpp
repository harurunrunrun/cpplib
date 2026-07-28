#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTS_EXACT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTS_EXACT_HPP_INCLUDED

#include <random>
#include <vector>

#include "../../detail/polyhedron/convex_polyhedron_intersects_detail.hpp"

inline bool convex_polyhedron_intersects_exact(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    using namespace convex_polyhedron_intersects_detail;
    std::vector<ExactHalfspace3> halfspaces =
        combined_exact_halfspaces(first, second);
    std::mt19937_64 random_engine = independent_random_engine();
    return randomized_exact_feasible(halfspaces, random_engine);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTS_EXACT_HPP_INCLUDED
