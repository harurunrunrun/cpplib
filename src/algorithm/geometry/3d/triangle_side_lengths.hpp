#pragma once

#include <array>

#include "abs.hpp"
#include "base.hpp"

inline std::array<long double, 3> triangle_side_lengths(
    const Triangle3& triangle
){
    return {
        abs(triangle.b - triangle.a),
        abs(triangle.c - triangle.b),
        abs(triangle.a - triangle.c),
    };
}
