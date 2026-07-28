#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_SEGMENT_INTERSECTION_RECORD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_SEGMENT_INTERSECTION_RECORD_HPP_INCLUDED

#include <cstddef>

#include "point.hpp"
#include "../../aggregate/all/segment_intersection_kind.hpp"

struct SegmentIntersectionRecord{
    std::size_t first;
    std::size_t second;
    SegmentIntersectionKind kind;
    Point first_point;
    Point second_point;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_SEGMENT_INTERSECTION_RECORD_HPP_INCLUDED
