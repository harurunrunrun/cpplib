#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "cross_point.hpp"
#include "dot.hpp"
#include "on_segment.hpp"

inline std::vector<Point> polygon_line_intersections(
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
    const auto append = [&](const Point& point){
        if(std::none_of(result.begin(), result.end(), [&](const Point& current){
            return current == point;
        })) result.push_back(point);
    };
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        const int first_side = geometry_sign(cross(direction, first - line.a));
        const int second_side = geometry_sign(cross(direction, second - line.a));
        if(first_side == 0) append(first);
        if(second_side == 0) append(second);
        if(first_side * second_side < 0){
            append(cross_point(line, Line{first, second}));
        }
    }
    std::sort(result.begin(), result.end(), [&](const Point& first, const Point& second){
        return dot(first - line.a, direction) < dot(second - line.a, direction);
    });
    return result;
}
