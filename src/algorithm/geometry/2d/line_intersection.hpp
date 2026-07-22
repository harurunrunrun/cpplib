#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_INTERSECTION_HPP_INCLUDED

#include "cross_point.hpp"

inline Point line_intersection(const Line& first, const Line& second){
    return cross_point(first, second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_INTERSECTION_HPP_INCLUDED
