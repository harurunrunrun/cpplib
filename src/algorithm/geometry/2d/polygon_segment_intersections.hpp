#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_SEGMENT_INTERSECTIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_SEGMENT_INTERSECTIONS_HPP_INCLUDED

#include <algorithm>
#include <vector>

#include "segment_intersection_kind.hpp"
#include "segment_intersection.hpp"

inline std::vector<Point> polygon_segment_intersections(
    const std::vector<Point>& polygon,
    const Segment& segment
){
    std::vector<Point> result;
    result.reserve(polygon.size() * 2);
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Segment edge{polygon[index], polygon[(index + 1) % polygon.size()]};
        const SegmentIntersectionKind kind = segment_intersection_kind(edge, segment);
        if(kind == SEGMENT_INTERSECTION_POINT){
            const std::optional<Segment> overlap = segment_overlap(edge, segment);
            if(overlap.has_value()) result.push_back(overlap->a);
            else result.push_back(segment_intersection(edge, segment));
        }else if(kind == SEGMENT_INTERSECTION_SEGMENT){
            const Segment overlap = *segment_overlap(edge, segment);
            result.push_back(overlap.a);
            result.push_back(overlap.b);
        }
    }
    const Point direction = segment.b - segment.a;
    std::stable_sort(result.begin(), result.end(), [&](const Point& first, const Point& second){
        return dot(first - segment.a, direction) < dot(second - segment.a, direction);
    });
    result.erase(std::unique(
        result.begin(), result.end(),
        [](const Point& first, const Point& second){ return first == second; }
    ), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POLYGON_SEGMENT_INTERSECTIONS_HPP_INCLUDED
