#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

inline Point cross_point(const Line& first, const Line& second){
    const Point first_direction = first.b - first.a;
    const Point second_direction = second.b - second.a;
    if(geometry_sign(abs(first_direction)) == 0 ||
        geometry_sign(abs(second_direction)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    const long double denominator = cross(first_direction, second_direction);
    if(geometry_sign(denominator) == 0)[[unlikely]]{
        throw std::domain_error("lines do not have a unique intersection");
    }
    return first.a + first_direction * (
        cross(second.a - first.a, second_direction) / denominator
    );
}
