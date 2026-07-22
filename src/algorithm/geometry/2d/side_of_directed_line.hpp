#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SIDE_OF_DIRECTED_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SIDE_OF_DIRECTED_LINE_HPP_INCLUDED

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

inline int side_of_directed_line(const Line& line, const Point& point){
    const Point direction = line.b - line.a;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const Point relative = point - line.a;
    const long double roundoff =
        std::abs(direction.x * relative.y) + std::abs(direction.y * relative.x);
    return geometry_scaled_sign(
        cross(direction, relative), length, roundoff
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SIDE_OF_DIRECTED_LINE_HPP_INCLUDED
