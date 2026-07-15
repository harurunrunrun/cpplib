#pragma once

#include <stdexcept>

#include "area.hpp"
#include "base.hpp"
#include "is_zero_vector.hpp"
#include "triangle_area_vector.hpp"
#include "triangle_side_lengths.hpp"

inline long double triangle_inradius(const Triangle3& triangle){
    if(is_zero_vector(triangle_area_vector(triangle)))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const auto lengths = triangle_side_lengths(triangle);
    return 2 * area(triangle) / (lengths[0] + lengths[1] + lengths[2]);
}
