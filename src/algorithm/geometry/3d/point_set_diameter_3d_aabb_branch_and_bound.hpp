#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_DIAMETER_3D_AABB_BRANCH_AND_BOUND_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_DIAMETER_3D_AABB_BRANCH_AND_BOUND_HPP_INCLUDED

#include <optional>
#include <vector>

#include "point_set_diameter_3d_detail.hpp"

inline std::optional<PointSetDiameter3DResult>
point_set_diameter_3d_aabb_branch_and_bound(
    const std::vector<Point3>& points
){
    point_set_diameter_3d_detail::validate(points);
    if(points.size() < 2) return std::nullopt;
    return point_set_diameter_3d_detail::AabbBranchAndBound(points).get_result();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_DIAMETER_3D_AABB_BRANCH_AND_BOUND_HPP_INCLUDED
