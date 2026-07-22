#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_CUT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_CUT_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "abs.hpp"
#include "cross.hpp"
#include "cross_point.hpp"

inline std::vector<Point> convex_cut(
    const std::vector<Point>& polygon,
    const Line& line
){
    const Point raw_direction = line.b - line.a;
    const long double direction_length = abs(raw_direction);
    if(geometry_sign(direction_length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const Point direction = raw_direction / direction_length;
    std::vector<Point> result;
    const int size = static_cast<int>(polygon.size());
    for(int index = 0; index < size; ++index){
        const Point current = polygon[static_cast<std::size_t>(index)];
        const Point next = polygon[
            static_cast<std::size_t>((index + 1) % size)
        ];
        const int current_side = geometry_sign(cross(
            direction, current - line.a
        ));
        const int next_side = geometry_sign(cross(
            direction, next - line.a
        ));
        if(current_side >= 0) result.push_back(current);
        if(current_side * next_side < 0){
            result.push_back(cross_point({current, next}, line));
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_CUT_HPP_INCLUDED
