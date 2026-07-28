#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_LINE_POINT_ON_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_LINE_POINT_ON_LINE_HPP_INCLUDED

#include "../../integer/point_point_point/orientation.hpp"

namespace integer_geometry{

inline bool on_line(const Line& line, const Point& point){
    return orientation(line.a, line.b, point) == 0;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_LINE_POINT_ON_LINE_HPP_INCLUDED
