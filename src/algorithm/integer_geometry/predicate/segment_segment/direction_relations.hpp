#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_SEGMENT_DIRECTION_RELATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_SEGMENT_DIRECTION_RELATIONS_HPP_INCLUDED

#include "../../vector/segment/direction_segment.hpp"
#include "../vector_vector/direction_relations.hpp"

namespace integer_geometry{

inline bool parallel(const Segment& first, const Segment& second){
    return parallel(direction(first), direction(second));
}

inline bool orthogonal(const Segment& first, const Segment& second){
    return orthogonal(direction(first), direction(second));
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_SEGMENT_DIRECTION_RELATIONS_HPP_INCLUDED
