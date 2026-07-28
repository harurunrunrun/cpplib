#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_POINT_POINT_VECTOR_FROM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_POINT_POINT_VECTOR_FROM_HPP_INCLUDED

#include "../../type/definition/geometry_types.hpp"

namespace integer_geometry{

inline Vector vector_from(const Point& from, const Point& to){
    return to - from;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_VECTOR_POINT_POINT_VECTOR_FROM_HPP_INCLUDED
