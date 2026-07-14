#pragma once

#include "integer_cross.hpp"

template<std::integral Coordinate>
inline int integer_orientation(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second,
    const IntegerPoint<Coordinate>& third
){
    const GeometryIntegerWide first_x =
        static_cast<__int128_t>(second.x) - static_cast<__int128_t>(first.x);
    const GeometryIntegerWide first_y =
        static_cast<__int128_t>(second.y) - static_cast<__int128_t>(first.y);
    const GeometryIntegerWide second_x =
        static_cast<__int128_t>(third.x) - static_cast<__int128_t>(first.x);
    const GeometryIntegerWide second_y =
        static_cast<__int128_t>(third.y) - static_cast<__int128_t>(first.y);
    const GeometryIntegerWide determinant =
        first_x * second_y - first_y * second_x;
    return (determinant > 0) - (determinant < 0);
}
