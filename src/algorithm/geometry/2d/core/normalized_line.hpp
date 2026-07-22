#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CORE_NORMALIZED_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CORE_NORMALIZED_LINE_HPP_INCLUDED

#include <stdexcept>

#include "../scalar/abs.hpp"

struct NormalizedLine{
    long double a;
    long double b;
    long double c;
};

inline NormalizedLine normalized_line(const Line& line){
    const Point direction = line.b - line.a;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    NormalizedLine result{
        direction.y / length,
        -direction.x / length,
        0.0L,
    };
    result.c = -result.a * line.a.x - result.b * line.a.y;
    if(result.a < 0.0L || (result.a == 0.0L && result.b < 0.0L)){
        result.a = -result.a;
        result.b = -result.b;
        result.c = -result.c;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CORE_NORMALIZED_LINE_HPP_INCLUDED
