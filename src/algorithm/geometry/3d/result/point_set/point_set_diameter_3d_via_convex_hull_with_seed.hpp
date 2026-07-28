#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_WITH_SEED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_WITH_SEED_HPP_INCLUDED

#include <cstdint>
#include <optional>
#include <vector>

#include "../../polyhedron/point_set/convex_hull_3d_with_seed.hpp"
#include "../../detail/point_set_polyhedron/point_set_diameter_3d_via_convex_hull_detail.hpp"

inline std::optional<PointSetDiameter3DResult>
point_set_diameter_3d_via_convex_hull_with_seed(
    const std::vector<Point3>& points,
    std::uint64_t random_seed
){
    return point_set_diameter_3d_via_convex_hull_detail::
        recover_original_indices(
            points, convex_hull_3d_with_seed(points, random_seed)
        );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_WITH_SEED_HPP_INCLUDED
