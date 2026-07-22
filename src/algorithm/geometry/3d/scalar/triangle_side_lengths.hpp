#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_SIDE_LENGTHS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_SIDE_LENGTHS_HPP_INCLUDED

#include <array>

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"

inline std::array<long double, 3> triangle_side_lengths(
    const Triangle3& triangle
){
    return {
        abs(triangle.b - triangle.a),
        abs(triangle.c - triangle.b),
        abs(triangle.a - triangle.c),
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TRIANGLE_SIDE_LENGTHS_HPP_INCLUDED
