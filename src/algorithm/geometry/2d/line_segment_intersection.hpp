#pragma once

#include <stdexcept>

#include "cross_point.hpp"
#include "intersect.hpp"
#include "intersect_line_segment.hpp"

inline Point line_segment_intersection(
    const Line& line,
    const Segment& segment
){
    const Point line_direction = line.b - line.a;
    const long double line_length = abs(line_direction);
    if(geometry_sign(line_length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const Point segment_direction = segment.b - segment.a;
    const long double segment_length = abs(segment_direction);
    if(geometry_sign(segment_length) == 0){
        if(intersect(line, segment.a)) return segment.a;
        throw std::domain_error("line and segment do not intersect");
    }
    if(!intersect_line_segment(line, segment)){
        throw std::domain_error("line and segment do not intersect");
    }
    if(geometry_sign(cross(
        line_direction / line_length,
        segment_direction / segment_length
    )) == 0){
        throw std::domain_error("line and segment do not have a unique intersection");
    }
    return cross_point(line, segment);
}
