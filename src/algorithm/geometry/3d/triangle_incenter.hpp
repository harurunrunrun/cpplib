#pragma once

#include <stdexcept>

#include "base.hpp"
#include "is_zero_vector.hpp"
#include "triangle_area_vector.hpp"
#include "triangle_side_lengths.hpp"

inline Point3 triangle_incenter(const Triangle3& triangle){
    if(is_zero_vector(triangle_area_vector(triangle)))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const auto lengths = triangle_side_lengths(triangle);
    const long double perimeter = lengths[0] + lengths[1] + lengths[2];
    return (
        triangle.c * lengths[0] +
        triangle.a * lengths[1] +
        triangle.b * lengths[2]
    ) / perimeter;
}
