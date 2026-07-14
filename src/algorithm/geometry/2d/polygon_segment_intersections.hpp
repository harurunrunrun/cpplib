#pragma once

#include <algorithm>
#include <vector>

#include "segment_intersection_kind.hpp"
#include "segment_intersection.hpp"

inline std::vector<Point> polygon_segment_intersections(
    const std::vector<Point>& polygon,
    const Segment& segment
){
    std::vector<Point> result;
    const auto append = [&](const Point& point){
        if(std::none_of(result.begin(), result.end(), [&](const Point& current){
            return current == point;
        })) result.push_back(point);
    };
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Segment edge{polygon[index], polygon[(index + 1) % polygon.size()]};
        const SegmentIntersectionKind kind = segment_intersection_kind(edge, segment);
        if(kind == SEGMENT_INTERSECTION_POINT){
            const std::optional<Segment> overlap = segment_overlap(edge, segment);
            if(overlap.has_value()) append(overlap->a);
            else append(segment_intersection(edge, segment));
        }else if(kind == SEGMENT_INTERSECTION_SEGMENT){
            const Segment overlap = *segment_overlap(edge, segment);
            append(overlap.a);
            append(overlap.b);
        }
    }
    const Point direction = segment.b - segment.a;
    std::sort(result.begin(), result.end(), [&](const Point& first, const Point& second){
        return dot(first - segment.a, direction) < dot(second - segment.a, direction);
    });
    return result;
}
