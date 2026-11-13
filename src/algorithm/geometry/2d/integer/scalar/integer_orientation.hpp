#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_SCALAR_INTEGER_ORIENTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_SCALAR_INTEGER_ORIENTATION_HPP_INCLUDED

#include <cstdint>
#include <type_traits>

#include "../../scalar/scalar/integer_cross.hpp"

template<std::integral Coordinate>
inline int integer_orientation(
    const IntegerPoint<Coordinate>& first,
    const IntegerPoint<Coordinate>& second,
    const IntegerPoint<Coordinate>& third
){
    if constexpr(
        std::is_signed_v<Coordinate>
            ? sizeof(Coordinate) <= sizeof(std::int64_t)
            : sizeof(Coordinate) <= sizeof(std::uint32_t)
    ){
        const __int128_t first_x =
            static_cast<__int128_t>(second.x) - static_cast<__int128_t>(first.x);
        const __int128_t first_y =
            static_cast<__int128_t>(second.y) - static_cast<__int128_t>(first.y);
        const __int128_t second_x =
            static_cast<__int128_t>(third.x) - static_cast<__int128_t>(first.x);
        const __int128_t second_y =
            static_cast<__int128_t>(third.y) - static_cast<__int128_t>(first.y);
        constexpr __int128_t limit = static_cast<__int128_t>(1) << 63;
        const auto fits = [](__int128_t value){
            return -limit <= value && value < limit;
        };
        if(fits(first_x) && fits(first_y) && fits(second_x) && fits(second_y)){
            // 差が int64 に収まれば各積の絶対値は 2^126 以下で、
            // 積どうしの比較は __int128_t で正確に行える。
            const __int128_t left = first_x * second_y;
            const __int128_t right = first_y * second_x;
            return (left > right) - (left < right);
        }
    }
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INTEGER_SCALAR_INTEGER_ORIENTATION_HPP_INCLUDED
