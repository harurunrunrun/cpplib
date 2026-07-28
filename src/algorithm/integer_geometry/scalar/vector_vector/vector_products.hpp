#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_VECTOR_VECTOR_VECTOR_PRODUCTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_VECTOR_VECTOR_VECTOR_PRODUCTS_HPP_INCLUDED

#include "../../type/definition/geometry_types.hpp"

namespace integer_geometry{

inline Rational dot(const Vector& first, const Vector& second){
    return first.x * second.x + first.y * second.y;
}

inline Rational cross(const Vector& first, const Vector& second){
    return first.x * second.y - first.y * second.x;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_VECTOR_VECTOR_VECTOR_PRODUCTS_HPP_INCLUDED
