#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_OVERLAP_HPP_INCLUDED

#include <variant>

#include "geometry_primitives.hpp"
#include "segment_segment_intersection.hpp"

inline bool overlap(const Segment3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        segment_segment_intersection(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_OVERLAP_HPP_INCLUDED
