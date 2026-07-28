#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_VECTOR_SQUARED_NORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_VECTOR_SQUARED_NORM_HPP_INCLUDED

#include "../vector_vector/vector_products.hpp"

namespace integer_geometry{

inline Rational squared_norm(const Vector& vector){
    return dot(vector, vector);
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_VECTOR_SQUARED_NORM_HPP_INCLUDED
