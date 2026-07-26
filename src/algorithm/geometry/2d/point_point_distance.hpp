#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_POINT_DISTANCE_HPP_INCLUDED

#include "abs.hpp"

inline long double distance(const Point& first, const Point& second){
    return abs(first - second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_POINT_DISTANCE_HPP_INCLUDED
