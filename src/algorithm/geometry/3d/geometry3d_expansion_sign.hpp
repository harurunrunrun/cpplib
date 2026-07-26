#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_SIGN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_SIGN_HPP_INCLUDED

#include "expansion_arithmetic_detail.hpp"

inline int geometry3d_expansion_sign(const Geometry3DExpansion& expansion){
    return geometry3d_expansion_detail::sign(expansion);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_SIGN_HPP_INCLUDED
