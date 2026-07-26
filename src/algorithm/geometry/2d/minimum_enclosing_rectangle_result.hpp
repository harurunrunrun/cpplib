#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_RECTANGLE_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_RECTANGLE_RESULT_HPP_INCLUDED

#include <array>

#include "point.hpp"

struct MinimumEnclosingRectangleResult{
    std::array<Point, 4> corners{};
    long double width = 0.0L;
    long double height = 0.0L;
    long double area = 0.0L;
    bool valid = false;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_MINIMUM_ENCLOSING_RECTANGLE_RESULT_HPP_INCLUDED
