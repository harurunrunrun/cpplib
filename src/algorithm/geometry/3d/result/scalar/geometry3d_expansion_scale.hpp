#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SCALAR_GEOMETRY3D_EXPANSION_SCALE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SCALAR_GEOMETRY3D_EXPANSION_SCALE_HPP_INCLUDED

#include "../../detail/numeric/expansion_arithmetic_detail.hpp"

inline Geometry3DExpansion geometry3d_expansion_scale(
    const Geometry3DExpansion& expansion,
    long double scalar
){
    return geometry3d_expansion_detail::scale(expansion, scalar);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SCALAR_GEOMETRY3D_EXPANSION_SCALE_HPP_INCLUDED
