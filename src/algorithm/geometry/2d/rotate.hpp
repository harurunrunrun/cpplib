#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ROTATE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ROTATE_HPP_INCLUDED

#include <cmath>

#include "types.hpp"

inline Point rotate(const Point& point, long double theta){
    const long double cosine = std::cos(theta);
    const long double sine = std::sin(theta);
    return {
        point.x * cosine - point.y * sine,
        point.x * sine + point.y * cosine,
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ROTATE_HPP_INCLUDED
