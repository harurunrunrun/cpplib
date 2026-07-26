#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CROSS_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CROSS_POINT_HPP_INCLUDED

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

inline Point cross_point(const Line& first, const Line& second){
    const Point first_direction = first.b - first.a;
    const Point second_direction = second.b - second.a;
    const long double first_length = abs(first_direction);
    const long double second_length = abs(second_direction);
    if(geometry_sign(first_length) == 0 ||
        geometry_sign(second_length) == 0){
        throw std::invalid_argument("degenerate line");
    }
    const Point first_unit = first_direction / first_length;
    const Point second_unit = second_direction / second_length;
    const long double denominator = cross(first_unit, second_unit);
    if(geometry_sign(denominator) == 0)[[unlikely]]{
        throw std::domain_error("lines do not have a unique intersection");
    }
    return first.a + first_unit * (
        cross(second.a - first.a, second_unit) / denominator
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CROSS_POINT_HPP_INCLUDED
