#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_SEGMENT_SEGMENT_SEGMENT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_SEGMENT_SEGMENT_SEGMENT_DISTANCE_HPP_INCLUDED

#include <algorithm>

#include "../../predicate/segment_segment/intersect_segment_segment.hpp"
#include "../segment_point/segment_point_distance.hpp"

inline long double distance(const Segment& first, const Segment& second){
    if(intersect(first, second)) return 0;
    return std::min({
        distance_segment_point(first, second.a),
        distance_segment_point(first, second.b),
        distance_segment_point(second, first.a),
        distance_segment_point(second, first.b),
    });
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SEGMENT_SEGMENT_SEGMENT_SEGMENT_DISTANCE_HPP_INCLUDED
