#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_CLOSEST_POINTS_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_CLOSEST_POINTS_LINE_RAY_HPP_INCLUDED

#include <utility>

#include "geometry_primitives.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& line,
    const Ray3& ray
){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {line.a, line.b, Kind::line},
        {ray.origin, ray.through, Kind::ray}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_CLOSEST_POINTS_LINE_RAY_HPP_INCLUDED
