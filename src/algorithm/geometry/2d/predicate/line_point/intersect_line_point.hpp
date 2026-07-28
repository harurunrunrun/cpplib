#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_POINT_INTERSECT_LINE_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_POINT_INTERSECT_LINE_POINT_HPP_INCLUDED

#include "../../integer/line_point/side_of_directed_line.hpp"

inline bool intersect(const Line& line, const Point& point){
    return side_of_directed_line(line, point) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_POINT_INTERSECT_LINE_POINT_HPP_INCLUDED
