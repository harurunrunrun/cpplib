#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_PERPENDICULAR_BISECTOR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_PERPENDICULAR_BISECTOR_HPP_INCLUDED

#include <stdexcept>

#include "../scalar/abs.hpp"
#include "../point/rotate90.hpp"

inline Line perpendicular_bisector(
    const Point& first,
    const Point& second
){
    const Point direction = second - first;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("two distinct points are required");
    }
    const Point midpoint = (first + second) / 2.0L;
    return {midpoint, midpoint + rotate90(direction)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_PERPENDICULAR_BISECTOR_HPP_INCLUDED
