#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_NORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_NORM_HPP_INCLUDED

#include "dot.hpp"

inline long double norm(const Point& point){
    return dot(point, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_NORM_HPP_INCLUDED
