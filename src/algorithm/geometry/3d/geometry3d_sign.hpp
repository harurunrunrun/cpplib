#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_SIGN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_SIGN_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

inline int geometry3d_sign(long double value){
    if(value > GEOMETRY3D_EPS) return 1;
    if(value < -GEOMETRY3D_EPS) return -1;
    return 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GEOMETRY3D_SIGN_HPP_INCLUDED
