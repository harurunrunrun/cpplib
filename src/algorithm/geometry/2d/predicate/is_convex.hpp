#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_IS_CONVEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_IS_CONVEX_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "../detail/advanced_convex_geometry_detail.hpp"

inline bool is_convex(const std::vector<Point>& polygon){
    const int size = static_cast<int>(polygon.size());
    int direction = 0;
    for(int index = 0; index < size; ++index){
        const int turn = advanced_geometry_detail::cross_sign(
            polygon[static_cast<std::size_t>((index + 1) % size)] -
                polygon[static_cast<std::size_t>(index)],
            polygon[static_cast<std::size_t>((index + 2) % size)] -
                polygon[static_cast<std::size_t>((index + 1) % size)]
        );
        if(turn == 0) continue;
        if(direction == 0) direction = turn;
        else if(direction != turn) return false;
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_IS_CONVEX_HPP_INCLUDED
