#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_EXACT_ORIENT3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_EXACT_ORIENT3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstdint>

#include "../../../math/integer/exact_integer.hpp"

using IntegerPoint3 = std::array<std::int64_t, 3>;

namespace geometry3d_integer_predicate_detail{

inline int sign(const ExactInteger& value){
    return value > 0 ? 1 : value < 0 ? -1 : 0;
}

inline unsigned __int128 unsigned_abs(__int128 value){
    return value < 0
        ? static_cast<unsigned __int128>(-(value + 1)) + 1
        : static_cast<unsigned __int128>(value);
}

inline int orient3d_exact_integer(
    const IntegerPoint3& first,
    const IntegerPoint3& second,
    const IntegerPoint3& third,
    const IntegerPoint3& fourth
){
    std::array<ExactInteger, 3> u{}, v{}, w{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        u[coordinate] = ExactInteger(second[coordinate]) - first[coordinate];
        v[coordinate] = ExactInteger(third[coordinate]) - first[coordinate];
        w[coordinate] = ExactInteger(fourth[coordinate]) - first[coordinate];
    }
    const ExactInteger determinant =
        u[0] * (v[1] * w[2] - v[2] * w[1])
        - u[1] * (v[0] * w[2] - v[2] * w[0])
        + u[2] * (v[0] * w[1] - v[1] * w[0]);
    return sign(determinant);
}

}  // namespace geometry3d_integer_predicate_detail

inline int exact_orient3d(
    const IntegerPoint3& first,
    const IntegerPoint3& second,
    const IntegerPoint3& third,
    const IntegerPoint3& fourth
){
    std::array<__int128, 3> u{}, v{}, w{};
    unsigned __int128 maximum = 0;
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        u[coordinate] = static_cast<__int128>(second[coordinate]) - first[coordinate];
        v[coordinate] = static_cast<__int128>(third[coordinate]) - first[coordinate];
        w[coordinate] = static_cast<__int128>(fourth[coordinate]) - first[coordinate];
        maximum = std::max(maximum, geometry3d_integer_predicate_detail::unsigned_abs(u[coordinate]));
        maximum = std::max(maximum, geometry3d_integer_predicate_detail::unsigned_abs(v[coordinate]));
        maximum = std::max(maximum, geometry3d_integer_predicate_detail::unsigned_abs(w[coordinate]));
    }
    // Six signed triple products fit comfortably when every difference is at most 2^40.
    if(maximum <= (static_cast<unsigned __int128>(1) << 40)){
        const __int128 determinant =
            u[0] * (v[1] * w[2] - v[2] * w[1])
            - u[1] * (v[0] * w[2] - v[2] * w[0])
            + u[2] * (v[0] * w[1] - v[1] * w[0]);
        return determinant > 0 ? 1 : determinant < 0 ? -1 : 0;
    }
    return geometry3d_integer_predicate_detail::orient3d_exact_integer(
        first, second, third, fourth
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_EXACT_ORIENT3D_HPP_INCLUDED
