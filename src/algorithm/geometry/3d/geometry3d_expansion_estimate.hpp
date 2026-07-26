#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_ESTIMATE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_ESTIMATE_HPP_INCLUDED

#include "expansion_arithmetic_detail.hpp"

inline long double geometry3d_expansion_estimate(
    const Geometry3DExpansion& expansion
){
    return geometry3d_expansion_detail::estimate(expansion);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_EXPANSION_ESTIMATE_HPP_INCLUDED
