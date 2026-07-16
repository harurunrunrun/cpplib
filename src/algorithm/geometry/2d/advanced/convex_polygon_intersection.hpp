#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <utility>
#include <vector>

#include "normalize_convex_polygon.hpp"

namespace convex_polygon_intersection_detail{

inline Line more_restrictive_half_plane(const Line& first, const Line& second){
    using namespace advanced_geometry_detail;
    return side_sign(first, second.a) > 0 ? second : first;
}

inline std::vector<Line> sorted_boundary_lines(const std::vector<Point>& polygon){
    using namespace advanced_geometry_detail;
    const std::size_t count = polygon.size();
    std::size_t first = 0;
    for(std::size_t index = 1; index < count; ++index){
        const Point direction = polygon[index] - polygon[(index + count - 1) % count];
        const Point first_direction =
            polygon[first] - polygon[(first + count - 1) % count];
        if(direction_less(direction, first_direction)) first = index;
    }

    std::vector<Line> result;
    result.reserve(count);
    for(std::size_t offset = 0; offset < count; ++offset){
        const std::size_t end = (first + offset) % count;
        const std::size_t begin = (end + count - 1) % count;
        result.push_back({polygon[begin], polygon[end]});
    }
    return result;
}

inline std::vector<Line> merge_boundary_lines(
    const std::vector<Point>& first,
    const std::vector<Point>& second
){
    using namespace advanced_geometry_detail;
    const std::vector<Line> first_lines = sorted_boundary_lines(first);
    const std::vector<Line> second_lines = sorted_boundary_lines(second);
    std::vector<Line> merged;
    merged.reserve(first_lines.size() + second_lines.size());

    std::size_t first_index = 0;
    std::size_t second_index = 0;
    while(first_index < first_lines.size() || second_index < second_lines.size()){
        Line next;
        if(first_index == first_lines.size()){
            next = second_lines[second_index++];
        }else if(second_index == second_lines.size()){
            next = first_lines[first_index++];
        }else{
            const Point first_direction =
                first_lines[first_index].b - first_lines[first_index].a;
            const Point second_direction =
                second_lines[second_index].b - second_lines[second_index].a;
            if(same_direction(first_direction, second_direction)){
                next = more_restrictive_half_plane(
                    first_lines[first_index++],
                    second_lines[second_index++]
                );
            }else if(direction_less(first_direction, second_direction)){
                next = first_lines[first_index++];
            }else{
                next = second_lines[second_index++];
            }
        }

        if(!merged.empty() && same_direction(
            merged.back().b - merged.back().a,
            next.b - next.a
        )){
            merged.back() = more_restrictive_half_plane(merged.back(), next);
        }else{
            merged.push_back(next);
        }
    }
    if(merged.size() >= 2 && same_direction(
        merged.front().b - merged.front().a,
        merged.back().b - merged.back().a
    )){
        merged.front() = more_restrictive_half_plane(merged.front(), merged.back());
        merged.pop_back();
    }
    return merged;
}

inline std::vector<Point> intersect_full_dimensional_polygons(
    const std::vector<Point>& first,
    const std::vector<Point>& second
){
    using namespace advanced_geometry_detail;
    const std::vector<Line> lines = merge_boundary_lines(first, second);
    std::deque<Line> active;
    const auto outside = [](const Line& line, const Point& point){
        return advanced_geometry_detail::side_sign(line, point) < 0;
    };

    for(const Line& line: lines){
        while(active.size() >= 2 && outside(
            line,
            line_intersection_unchecked(active[active.size() - 2], active.back())
        )){
            active.pop_back();
        }
        while(active.size() >= 2 && outside(
            line,
            line_intersection_unchecked(active[0], active[1])
        )){
            active.pop_front();
        }
        if(!active.empty() && parallel_direction(
            active.back().b - active.back().a,
            line.b - line.a
        )){
            return {};
        }
        active.push_back(line);
    }

    while(active.size() >= 3 && outside(
        active.front(),
        line_intersection_unchecked(active[active.size() - 2], active.back())
    )){
        active.pop_back();
    }
    while(active.size() >= 3 && outside(
        active.back(),
        line_intersection_unchecked(active[0], active[1])
    )){
        active.pop_front();
    }
    if(active.size() < 3 || parallel_direction(
        active.front().b - active.front().a,
        active.back().b - active.back().a
    )){
        return {};
    }

    std::vector<Point> polygon;
    polygon.reserve(active.size());
    for(std::size_t index = 0; index < active.size(); ++index){
        const Line& current = active[index];
        const Line& next = active[(index + 1) % active.size()];
        if(parallel_direction(current.b - current.a, next.b - next.a)) return {};
        polygon.push_back(line_intersection_unchecked(current, next));
    }
    return normalize_convex_polygon(std::move(polygon));
}

}  // namespace convex_polygon_intersection_detail

inline std::vector<Point> convex_polygon_intersection(
    std::vector<Point> first,
    std::vector<Point> second
){
    using namespace advanced_geometry_detail;

    first = normalize_convex_polygon(std::move(first));
    second = normalize_convex_polygon(std::move(second));
    if(first.empty() || second.empty()) return {};
    if(first.size() == 1){
        return point_in_normalized_convex_polygon(second, first.front())
            ? first
            : std::vector<Point>{};
    }
    if(second.size() == 1){
        return point_in_normalized_convex_polygon(first, second.front())
            ? second
            : std::vector<Point>{};
    }
    if(first.size() == 2 && second.size() == 2){
        return segment_intersection_set(
            {first[0], first[1]},
            {second[0], second[1]}
        );
    }
    if(first.size() == 2){
        return clip_segment_by_convex_polygon({first[0], first[1]}, second);
    }
    if(second.size() == 2){
        return clip_segment_by_convex_polygon({second[0], second[1]}, first);
    }

    return convex_polygon_intersection_detail::intersect_full_dimensional_polygons(
        first, second
    );
}
