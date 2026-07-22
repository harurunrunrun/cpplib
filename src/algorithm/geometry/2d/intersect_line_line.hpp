#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTERSECT_LINE_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTERSECT_LINE_LINE_HPP_INCLUDED

#include "same_line.hpp"

inline bool intersect_line_line(const Line& first, const Line& second){
    return !parallel(first, second) || same_line(first, second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTERSECT_LINE_LINE_HPP_INCLUDED
