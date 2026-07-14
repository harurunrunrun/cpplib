#pragma once

#include <array>
#include <stdexcept>

#include "advanced/detail.hpp"
#include "cross_point.hpp"
#include "unit.hpp"

inline std::array<Line, 2> angle_bisectors(
    const Line& first,
    const Line& second
){
    Point first_direction = unit(first.b - first.a);
    Point second_direction = unit(second.b - second.a);
    if(advanced_geometry_detail::cross_sign(
        first_direction, second_direction
    ) == 0){
        throw std::domain_error("parallel lines do not define two finite angle bisectors");
    }
    if(first_direction.x < 0.0L ||
       (first_direction.x == 0.0L && first_direction.y < 0.0L)){
        first_direction = -first_direction;
    }
    if(second_direction.x < 0.0L ||
       (second_direction.x == 0.0L && second_direction.y < 0.0L)){
        second_direction = -second_direction;
    }
    const Point intersection = cross_point(first, second);
    return {
        Line{intersection, intersection + unit(first_direction + second_direction)},
        Line{intersection, intersection + unit(first_direction - second_direction)},
    };
}
