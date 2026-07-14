#pragma once

#include <cstddef>
#include <vector>

#include "advanced/detail.hpp"

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
