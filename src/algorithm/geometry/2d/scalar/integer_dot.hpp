#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_INTEGER_DOT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_INTEGER_DOT_HPP_INCLUDED

#include "../../../math/integer/exact_integer.hpp"

#include "../core/integer_point.hpp"

using GeometryIntegerWide = ExactInteger;

template<std::integral Coordinate>
inline GeometryIntegerWide integer_dot(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second
){
    const __int128_t first_x = static_cast<__int128_t>(first.x);
    const __int128_t first_y = static_cast<__int128_t>(first.y);
    const __int128_t second_x = static_cast<__int128_t>(second.x);
    const __int128_t second_y = static_cast<__int128_t>(second.y);
    return GeometryIntegerWide(first_x) * GeometryIntegerWide(second_x) +
        GeometryIntegerWide(first_y) * GeometryIntegerWide(second_y);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_INTEGER_DOT_HPP_INCLUDED
