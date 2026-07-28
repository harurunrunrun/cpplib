#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_POINT_ON_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_POINT_ON_SEGMENT_HPP_INCLUDED

#include <algorithm>

#include "../../integer/point_point_point/orientation.hpp"

namespace integer_geometry{

inline bool on_segment(const Segment& segment, const Point& point){
    if(orientation(segment.a, segment.b, point) != 0) return false;
    return std::min(segment.a.x, segment.b.x) <= point.x
        && point.x <= std::max(segment.a.x, segment.b.x)
        && std::min(segment.a.y, segment.b.y) <= point.y
        && point.y <= std::max(segment.a.y, segment.b.y);
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_POINT_ON_SEGMENT_HPP_INCLUDED
