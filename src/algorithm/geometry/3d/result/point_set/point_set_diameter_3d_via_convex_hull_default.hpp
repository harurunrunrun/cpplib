#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_DEFAULT_HPP_INCLUDED

#include <optional>
#include <vector>

#include "../../polyhedron/point_set/convex_hull_3d_default.hpp"
#include "../../detail/point_set_polyhedron/point_set_diameter_3d_via_convex_hull_detail.hpp"

inline std::optional<PointSetDiameter3DResult>
point_set_diameter_3d_via_convex_hull(
    const std::vector<Point3>& points
){
    return point_set_diameter_3d_via_convex_hull_detail::
        recover_original_indices(points, convex_hull_3d(points));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_VIA_CONVEX_HULL_DEFAULT_HPP_INCLUDED
