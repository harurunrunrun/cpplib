#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_SEGMENT_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_SEGMENT_OVERLAP_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <optional>

#include "../scalar/abs.hpp"
#include "../predicate/on_segment.hpp"
#include "../predicate/side_of_directed_line.hpp"

inline std::optional<Segment> segment_overlap(
    const Segment& first,
    const Segment& second
){
    const bool first_is_point =
        geometry_sign(abs(first.b - first.a)) == 0;
    const bool second_is_point =
        geometry_sign(abs(second.b - second.a)) == 0;
    if(first_is_point){
        if(on_segment(second, first.a)) return Segment{first.a, first.a};
        return std::nullopt;
    }
    if(second_is_point){
        if(on_segment(first, second.a)) return Segment{second.a, second.a};
        return std::nullopt;
    }
    if(side_of_directed_line(first, second.a) != 0 ||
       side_of_directed_line(first, second.b) != 0){
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_SEGMENT_OVERLAP_HPP_INCLUDED
