#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SEGMENT_POINT_SEGMENT_POINT_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SEGMENT_POINT_SEGMENT_POINT_CLOSEST_POINT_HPP_INCLUDED

#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../type/definition/point3.hpp"
#include "../../type/definition/segment3.hpp"

inline Point3 closest_point(const Segment3& segment, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    return query(
        {segment.a, segment.b, Kind::segment},
        {point, point, Kind::segment}
    ).first;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SEGMENT_POINT_SEGMENT_POINT_CLOSEST_POINT_HPP_INCLUDED
