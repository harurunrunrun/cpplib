#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_DEFAULT_HPP_INCLUDED

#include <optional>
#include <vector>

#include "point_set_diameter_3d_aabb_branch_and_bound.hpp"

inline std::optional<PointSetDiameter3DResult> point_set_diameter_3d(
    const std::vector<Point3>& points
){
    return point_set_diameter_3d_aabb_branch_and_bound(points);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_SET_POINT_SET_DIAMETER_3D_DEFAULT_HPP_INCLUDED
