#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SCALAR_INTEGER_CROSS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SCALAR_INTEGER_CROSS_HPP_INCLUDED

#include <cstdint>
#include <type_traits>

#include "../../type/definition/geometry_integer_wide.hpp"
#include "../../type/definition/integer_point.hpp"

template<std::integral Coordinate>
inline GeometryIntegerWide integer_cross(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second
){
    if constexpr(
        std::is_signed_v<Coordinate>
            ? sizeof(Coordinate) <= sizeof(std::int64_t)
            : sizeof(Coordinate) <= sizeof(std::uint32_t)
    ){
        // 各積の絶対値は 2^126 以下、差の絶対値は 2^127 - 2^63 以下で
        // __int128_t に収まる。
        return GeometryIntegerWide(
            static_cast<__int128_t>(first.x) *
                static_cast<__int128_t>(second.y) -
            static_cast<__int128_t>(first.y) *
                static_cast<__int128_t>(second.x)
        );
    }
    const __int128_t first_x = static_cast<__int128_t>(first.x);
    const __int128_t first_y = static_cast<__int128_t>(first.y);
    const __int128_t second_x = static_cast<__int128_t>(second.x);
    const __int128_t second_y = static_cast<__int128_t>(second.y);
    return GeometryIntegerWide(first_x) * GeometryIntegerWide(second_y) -
        GeometryIntegerWide(first_y) * GeometryIntegerWide(second_x);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_SCALAR_INTEGER_CROSS_HPP_INCLUDED
