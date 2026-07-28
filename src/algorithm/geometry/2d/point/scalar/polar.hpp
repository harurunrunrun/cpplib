#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SCALAR_POLAR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SCALAR_POLAR_HPP_INCLUDED

#include <cmath>

#include "../../aggregate/all/types.hpp"

inline Point polar(long double radius, long double theta){
    return {radius * std::cos(theta), radius * std::sin(theta)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SCALAR_POLAR_HPP_INCLUDED
