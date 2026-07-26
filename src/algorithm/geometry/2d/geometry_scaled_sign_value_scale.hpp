#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_GEOMETRY_SCALED_SIGN_VALUE_SCALE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_GEOMETRY_SCALED_SIGN_VALUE_SCALE_HPP_INCLUDED

#include "geometry_scaled_sign_value_tolerance_roundoff_scales.hpp"

inline int geometry_scaled_sign(long double value, long double scale){
    return geometry_scaled_sign(value, scale, scale);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_GEOMETRY_SCALED_SIGN_VALUE_SCALE_HPP_INCLUDED
