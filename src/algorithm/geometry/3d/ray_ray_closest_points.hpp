#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_CLOSEST_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_CLOSEST_POINTS_HPP_INCLUDED

#include <utility>

#include "base.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& first,
    const Ray3& second
){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {first.origin, first.through, Kind::ray},
        {second.origin, second.through, Kind::ray}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_CLOSEST_POINTS_HPP_INCLUDED
