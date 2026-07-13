#pragma once

#include <cstddef>
#include <vector>

#include "cross.hpp"

inline long double area(const std::vector<Point>& polygon){
    long double result = 0;
    const int size = static_cast<int>(polygon.size());
    for(int index = 0; index < size; ++index){
        result += cross(
            polygon[static_cast<std::size_t>(index)],
            polygon[static_cast<std::size_t>((index + 1) % size)]
        );
    }
    return result / 2;
}
