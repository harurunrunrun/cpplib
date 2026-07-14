#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "cross.hpp"
#include "on_segment.hpp"

inline int contains(const std::vector<Point>& polygon, const Point& point){
    bool inside = false;
    const int size = static_cast<int>(polygon.size());
    for(int index = 0; index < size; ++index){
        const Point& first_vertex =
            polygon[static_cast<std::size_t>(index)];
        const Point& second_vertex = polygon[
            static_cast<std::size_t>((index + 1) % size)
        ];
        if(on_segment({first_vertex, second_vertex}, point)) return 1;

        Point first = first_vertex - point;
        Point second = second_vertex - point;
        if(first.y > second.y) std::swap(first, second);
        if(first.y <= 0.0L && second.y > 0.0L &&
            cross(first, second) > 0.0L){
            inside = !inside;
        }
    }
    return inside ? 2 : 0;
}
