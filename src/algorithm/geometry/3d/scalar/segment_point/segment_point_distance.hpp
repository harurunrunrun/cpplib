#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_POINT_SEGMENT_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_POINT_SEGMENT_POINT_DISTANCE_HPP_INCLUDED

#include "../../aggregate/all/closest_point.hpp"
#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/segment3.hpp"

inline long double distance(const Segment3& segment, const Point3& point){
    return geometry3d_linear_closest_detail::checked_distance(
        point, closest_point(segment, point)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_POINT_SEGMENT_POINT_DISTANCE_HPP_INCLUDED
