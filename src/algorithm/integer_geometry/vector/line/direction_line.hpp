#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_LINE_DIRECTION_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_LINE_DIRECTION_LINE_HPP_INCLUDED

#include "../../type/definition/geometry_types.hpp"

namespace integer_geometry{

inline Vector direction(const Line& line){ return line.b - line.a; }

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_LINE_DIRECTION_LINE_HPP_INCLUDED
