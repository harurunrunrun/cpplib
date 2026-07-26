#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_UNARY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_UNARY_HPP_INCLUDED

#include <utility>

#include "expansion_arithmetic_detail.hpp"

inline Geometry3DExpansion geometry3d_expansion_negate(
    Geometry3DExpansion expansion
){
    return geometry3d_expansion_detail::negate(std::move(expansion));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_UNARY_HPP_INCLUDED
