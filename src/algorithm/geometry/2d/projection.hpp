#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PROJECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PROJECTION_HPP_INCLUDED

#include <stdexcept>

#include "abs.hpp"
#include "dot.hpp"
#include "norm.hpp"

inline Point projection(const Line& line, const Point& point){
    const Point direction = line.b - line.a;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    return line.a + direction * (dot(point - line.a, direction) / norm(direction));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PROJECTION_HPP_INCLUDED
