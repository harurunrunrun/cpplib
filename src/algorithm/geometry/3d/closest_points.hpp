#pragma once

#include <utility>

#include "base.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& first,
    const Line3& second
){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {first.a, first.b, Kind::line},
        {second.a, second.b, Kind::line}
    );
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& first,
    const Segment3& second
){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {first.a, first.b, Kind::segment},
        {second.a, second.b, Kind::segment}
    );
}
