#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_WITH_SEED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_WITH_SEED_HPP_INCLUDED

#include <cstdint>
#include <utility>
#include <vector>

#include "../../detail/point_set/convex_hull_3d_detail.hpp"

inline ConvexPolyhedron3 convex_hull_3d_with_seed(
    std::vector<Point3> input,
    std::uint64_t random_seed
){
    return convex_hull_3d_detail::build_with_seed(
        std::move(input), random_seed
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POINT_SET_CONVEX_HULL_3D_WITH_SEED_HPP_INCLUDED
