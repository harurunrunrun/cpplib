#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_SEGMENT_INTERSECTION_HPP_INCLUDED

#include "polygon3_line_intersection.hpp"

inline Polygon3LinearIntersection3 polygon3_segment_intersection(
    const Polygon3& polygon,
    const Segment3& segment
){
    if(segment.a == segment.b){
        Polygon3LinearIntersection3 result;
        if(polygon3_contains(polygon, segment.a) != 0){
            result.points.push_back(segment.a);
        }
        return result;
    }
    return polygon3_line_intersection_detail::intersect(
        polygon, {segment.a, segment.b}, 0.0L, 1.0L
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_SEGMENT_INTERSECTION_HPP_INCLUDED
