#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_NORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_NORM_HPP_INCLUDED

#include "../point_point/dot.hpp"

inline long double norm(const Point& point){
    return dot(point, point);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_NORM_HPP_INCLUDED
