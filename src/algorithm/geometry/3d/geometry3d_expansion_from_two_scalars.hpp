#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_FROM_TWO_SCALARS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_FROM_TWO_SCALARS_HPP_INCLUDED

#include "expansion_arithmetic_detail.hpp"

inline Geometry3DExpansion geometry3d_two_sum(
    long double first,
    long double second
){
    return geometry3d_expansion_detail::two_sum(first, second);
}

inline Geometry3DExpansion geometry3d_two_diff(
    long double first,
    long double second
){
    return geometry3d_expansion_detail::two_diff(first, second);
}

inline Geometry3DExpansion geometry3d_two_product(
    long double first,
    long double second
){
    return geometry3d_expansion_detail::two_product(first, second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_FROM_TWO_SCALARS_HPP_INCLUDED
