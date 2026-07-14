#pragma once

#include <algorithm>
#include <boost/multiprecision/cpp_int.hpp>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <vector>

#include "integer_point.hpp"

namespace lattice_polygon_detail{

using Wide = boost::multiprecision::int256_t;

template<std::integral Coordinate>
inline __uint128_t unsigned_difference(Coordinate first, Coordinate second){
    const __int128_t difference =
        static_cast<__int128_t>(first) - static_cast<__int128_t>(second);
    return difference < 0
        ? static_cast<__uint128_t>(-difference)
        : static_cast<__uint128_t>(difference);
}

inline __uint128_t gcd(__uint128_t first, __uint128_t second){
    while(second != 0){
        const __uint128_t remainder = first % second;
        first = second;
        second = remainder;
    }
    return first;
}

template<std::integral Coordinate>
inline Wide signed_double_area(
    const std::vector<IntegerPoint<Coordinate>>& polygon
){
    Wide result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const auto& current = polygon[index];
        const auto& next = polygon[(index + 1) % polygon.size()];
        const __int128_t current_x = static_cast<__int128_t>(current.x);
        const __int128_t current_y = static_cast<__int128_t>(current.y);
        const __int128_t next_x = static_cast<__int128_t>(next.x);
        const __int128_t next_y = static_cast<__int128_t>(next.y);
        result += Wide(current_x) * Wide(next_y) -
            Wide(current_y) * Wide(next_x);
    }
    return result;
}

inline __uint128_t to_uint128(const Wide& value){
    if(value < 0 || value > Wide((std::numeric_limits<__uint128_t>::max)())){
        throw std::overflow_error("lattice point count does not fit __uint128_t");
    }
    return value.convert_to<__uint128_t>();
}

} // namespace lattice_polygon_detail
