#pragma once

#include <algorithm>
#include <array>
#include <optional>

#include "on_segment.hpp"

inline std::optional<Segment> segment_overlap(
    const Segment& first,
    const Segment& second
){
    if(geometry_sign(cross(first.b - first.a, second.a - first.a)) != 0 ||
       geometry_sign(cross(first.b - first.a, second.b - first.a)) != 0){
        return std::nullopt;
    }
    Point first_low = first.a;
    Point first_high = first.b;
    Point second_low = second.a;
    Point second_high = second.b;
    if(first_high < first_low) std::swap(first_low, first_high);
    if(second_high < second_low) std::swap(second_low, second_high);
    const Point low = std::max(first_low, second_low);
    const Point high = std::min(first_high, second_high);
    if(high < low && !(high == low)) return std::nullopt;
    if(!on_segment(first, low) || !on_segment(second, low) ||
       !on_segment(first, high) || !on_segment(second, high)){
        return std::nullopt;
    }
    return Segment{low, high};
}
