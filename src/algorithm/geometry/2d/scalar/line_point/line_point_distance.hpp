#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_LINE_POINT_LINE_POINT_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_LINE_POINT_LINE_POINT_DISTANCE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../point/abs.hpp"
#include "../point_point/cross.hpp"

inline long double distance(const Line& line, const Point& point){
    const Point direction = line.b - line.a;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    return std::abs(cross(direction, point - line.a)) / abs(direction);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_LINE_POINT_LINE_POINT_DISTANCE_HPP_INCLUDED
