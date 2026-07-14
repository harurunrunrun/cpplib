#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"
#include "on_segment.hpp"
#include "side_of_directed_line.hpp"

inline bool intersect(const Segment& first, const Segment& second){
    const auto side = [](const Segment& segment, const Point& point){
        if(geometry_sign(abs(segment.b - segment.a)) == 0) return 0;
        return side_of_directed_line(segment, point);
    };
    const int first_a = side(first, second.a);
    const int first_b = side(first, second.b);
    const int second_a = side(second, first.a);
    const int second_b = side(second, first.b);
    if(first_a == 0 && on_segment(first, second.a)) return true;
    if(first_b == 0 && on_segment(first, second.b)) return true;
    if(second_a == 0 && on_segment(second, first.a)) return true;
    if(second_b == 0 && on_segment(second, first.b)) return true;
    return first_a * first_b < 0 && second_a * second_b < 0;
}

inline bool intersect(const Line& line, const Point& point){
    return side_of_directed_line(line, point) == 0;
}
