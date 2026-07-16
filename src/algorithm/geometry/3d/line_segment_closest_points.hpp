#pragma once

#include <utility>

#include "base.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& line,
    const Segment3& segment
){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {line.a, line.b, Kind::line},
        {segment.a, segment.b, Kind::segment}
    );
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Line3& line
){
    const auto result = closest_points(line, segment);
    return {result.second, result.first};
}
