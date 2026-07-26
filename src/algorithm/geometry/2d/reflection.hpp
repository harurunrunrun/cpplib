#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_REFLECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_REFLECTION_HPP_INCLUDED

#include "projection.hpp"

inline Point reflection(const Line& line, const Point& point){
    return point + (projection(line, point) - point) * 2;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_REFLECTION_HPP_INCLUDED
