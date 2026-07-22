#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "abs.hpp"
#include "cross.hpp"
#include "cross_point.hpp"
#include "intersect.hpp"
#include "on_segment.hpp"
#include "parallel.hpp"

inline Point segment_intersection(
    const Segment& first,
    const Segment& second
){
    if(!intersect(first, second))[[unlikely]]{
        throw std::domain_error("segments do not intersect");
    }
    const Point first_direction = first.b - first.a;
    const Point second_direction = second.b - second.a;
    if(geometry_sign(abs(first_direction)) != 0 &&
        geometry_sign(abs(second_direction)) != 0 &&
        !parallel(first_direction, second_direction)){
        return cross_point(first, second);
    }
    std::vector<Point> candidates;
    for(const Point& point: {first.a, first.b, second.a, second.b}){
        if(on_segment(first, point) && on_segment(second, point) &&
            std::none_of(candidates.begin(), candidates.end(),
                [&](const Point& candidate){ return point == candidate; })){
            candidates.push_back(point);
        }
    }
    if(candidates.size() == 1) return candidates[0];
    throw std::domain_error("segments do not have a unique intersection");
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_INTERSECTION_HPP_INCLUDED
