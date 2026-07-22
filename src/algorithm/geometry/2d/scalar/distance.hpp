#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_DISTANCE_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "../predicate/intersect.hpp"
#include "../predicate/intersect_line_line.hpp"
#include "../predicate/intersect_line_segment.hpp"

inline long double distance(const Point& first, const Point& second){
    return abs(first - second);
}

inline long double distance(const Line& line, const Point& point){
    const Point direction = line.b - line.a;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    return std::abs(cross(direction, point - line.a)) / abs(direction);
}

inline long double distance_segment_point(
    const Segment& segment,
    const Point& point
){
    const Point direction = segment.b - segment.a;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0){
        return abs(point - segment.a);
    }
    const Point unit_direction = direction / length;
    const long double projection = dot(point - segment.a, unit_direction);
    if(geometry_sign(projection) < 0){
        return abs(point - segment.a);
    }
    if(geometry_sign(projection - length) > 0){
        return abs(point - segment.b);
    }
    return std::abs(cross(unit_direction, point - segment.a));
}

inline long double distance(const Segment& first, const Segment& second){
    if(intersect(first, second)) return 0;
    return std::min({
        distance_segment_point(first, second.a),
        distance_segment_point(first, second.b),
        distance_segment_point(second, first.a),
        distance_segment_point(second, first.b),
    });
}

inline long double distance_line_line(const Line& first, const Line& second){
    if(intersect_line_line(first, second)) return 0;
    return distance(first, second.a);
}

inline long double distance_line_segment(const Line& line, const Segment& segment){
    if(intersect_line_segment(line, segment)) return 0;
    return std::min(distance(line, segment.a), distance(line, segment.b));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_DISTANCE_HPP_INCLUDED
