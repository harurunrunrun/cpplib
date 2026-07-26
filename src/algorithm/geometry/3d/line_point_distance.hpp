#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_DISTANCE_HPP_INCLUDED

#include "line3.hpp"
#include "line_point_projection.hpp"
#include "linear_closest_detail.hpp"
#include "point3.hpp"

inline long double distance(const Line3& line, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, projection(line, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_POINT_DISTANCE_HPP_INCLUDED
