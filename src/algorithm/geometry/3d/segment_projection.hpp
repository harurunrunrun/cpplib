#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PROJECTION_HPP_INCLUDED

#include "base.hpp"
#include "closest_point.hpp"

inline Point3 projection(const Segment3& segment, const Point3& point){
    return closest_point(segment, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PROJECTION_HPP_INCLUDED
