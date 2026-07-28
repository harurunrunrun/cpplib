#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_MINIMUM_ENCLOSING_ELLIPSE_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_MINIMUM_ENCLOSING_ELLIPSE_RESULT_HPP_INCLUDED

#include <array>

#include "point.hpp"

struct MinimumEnclosingEllipseResult{
    Point center{};
    std::array<std::array<long double, 2>, 2> quadratic_form{{
        {{0.0L, 0.0L}},
        {{0.0L, 0.0L}},
    }};
    long double approximation_tolerance = 0.0L;

    long double evaluate(const Point& point) const{
        const Point difference = point - center;
        return difference.x * (
            quadratic_form[0][0] * difference.x
            + quadratic_form[0][1] * difference.y
        ) + difference.y * (
            quadratic_form[1][0] * difference.x
            + quadratic_form[1][1] * difference.y
        );
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_MINIMUM_ENCLOSING_ELLIPSE_RESULT_HPP_INCLUDED
