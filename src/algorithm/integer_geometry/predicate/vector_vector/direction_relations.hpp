#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_VECTOR_VECTOR_DIRECTION_RELATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_VECTOR_VECTOR_DIRECTION_RELATIONS_HPP_INCLUDED

#include "../../scalar/vector_vector/vector_products.hpp"

namespace integer_geometry{

inline bool parallel(const Vector& first, const Vector& second){
    return cross(first, second).is_zero();
}

inline bool orthogonal(const Vector& first, const Vector& second){
    return dot(first, second).is_zero();
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_VECTOR_VECTOR_DIRECTION_RELATIONS_HPP_INCLUDED
