#pragma once

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
