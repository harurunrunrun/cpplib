#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_MIXED_GEOMETRY3D_EXPANSION_BINARY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_MIXED_GEOMETRY3D_EXPANSION_BINARY_HPP_INCLUDED

#include "../../detail/numeric/expansion_arithmetic_detail.hpp"

inline Geometry3DExpansion geometry3d_expansion_sum(
    const Geometry3DExpansion& first,
    const Geometry3DExpansion& second
){
    return geometry3d_expansion_detail::sum(first, second);
}

inline Geometry3DExpansion geometry3d_expansion_product(
    const Geometry3DExpansion& first,
    const Geometry3DExpansion& second
){
    return geometry3d_expansion_detail::product(first, second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_MIXED_GEOMETRY3D_EXPANSION_BINARY_HPP_INCLUDED
