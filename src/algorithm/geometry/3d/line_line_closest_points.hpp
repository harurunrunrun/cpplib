#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_CLOSEST_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_CLOSEST_POINTS_HPP_INCLUDED

#include <utility>

#include "line3.hpp"
#include "linear_closest_detail.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_CLOSEST_POINTS_HPP_INCLUDED
