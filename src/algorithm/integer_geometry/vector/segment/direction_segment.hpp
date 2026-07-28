#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_SEGMENT_DIRECTION_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_SEGMENT_DIRECTION_SEGMENT_HPP_INCLUDED

#include "../../type/definition/geometry_types.hpp"

namespace integer_geometry{

inline Vector direction(const Segment& segment){ return segment.b - segment.a; }

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_SEGMENT_DIRECTION_SEGMENT_HPP_INCLUDED
