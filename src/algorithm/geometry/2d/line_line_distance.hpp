#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_LINE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_LINE_DISTANCE_HPP_INCLUDED

#include "intersect_line_line.hpp"
#include "line_point_distance.hpp"

inline long double distance_line_line(const Line& first, const Line& second){
    if(intersect_line_line(first, second)) return 0;
    return distance(first, second.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_LINE_DISTANCE_HPP_INCLUDED
