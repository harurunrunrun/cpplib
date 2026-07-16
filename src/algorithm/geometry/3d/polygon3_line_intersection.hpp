#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

#include "../2d/abs.hpp"
#include "../2d/cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line3_direction.hpp"
#include "line_plane_intersection.hpp"
#include "line_segment_intersection.hpp"
#include "on_plane.hpp"
#include "polygon3_coplanar.hpp"
#include "polygon3_contains.hpp"
#include "polygon3_normal.hpp"
#include "polygon3_to_2d.hpp"

struct Polygon3LinearIntersection3{
    std::vector<Point3> points;
    std::vector<Segment3> segments;

    bool empty() const noexcept{ return points.empty() && segments.empty(); }
};

namespace polygon3_line_intersection_detail{

inline long double parameter(
    const Line3& line,
    const Point3& direction,
    const Point3& point
){
    const std::array<long double, 3> absolute{
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    };
    const std::size_t axis = static_cast<std::size_t>(
        std::max_element(absolute.begin(), absolute.end()) - absolute.begin()
    );
    if(axis == 0) return (point.x - line.a.x) / direction.x;
    if(axis == 1) return (point.y - line.a.y) / direction.y;
    return (point.z - line.a.z) / direction.z;
}

inline Point3 at(const Line3& line, const Point3& direction, long double value){
    return line.a + direction * value;
}

inline bool near(long double first, long double second){
    return std::abs(first - second) <= 128.0L
        * std::numeric_limits<long double>::epsilon()
        * std::max({1.0L, std::abs(first), std::abs(second)});
}

inline std::size_t event_index(
    const std::vector<long double>& events,
    long double value
){
    auto iterator = std::lower_bound(events.begin(), events.end(), value);
    if(iterator != events.end() && near(*iterator, value)){
        return static_cast<std::size_t>(iterator - events.begin());
    }
    if(iterator != events.begin()){
        --iterator;
        if(near(*iterator, value)){
            return static_cast<std::size_t>(iterator - events.begin());
        }
    }
    throw std::logic_error("polygon3 line event was not registered");
}

inline Polygon3LinearIntersection3 intersect(
    const Polygon3& polygon,
    const Line3& line,
    long double lower,
    long double upper
){
    if(polygon.size() < 3 || !polygon3_coplanar(polygon))[[unlikely]]{
        throw std::invalid_argument(
            "polygon3 linear intersection requires a planar polygon"
        );
    }
    const Point3 direction = line3_direction(line);
    const Plane3 plane{polygon.front(), polygon3_normal(polygon)};
    Polygon3LinearIntersection3 result;
    if(geometry3d_sign(dot(direction, plane.normal)) != 0){
        const Point3 point = line_plane_intersection(line, plane);
        const long double value = parameter(line, direction, point);
        if(value >= lower && value <= upper
            && polygon3_contains(polygon, point) != 0){
            result.points.push_back(point);
        }
        return result;
    }
    if(!on_plane(plane, line.a)) return result;

    const PlaneCoordinateSystem3 system = plane_coordinate_system(plane);
    const std::vector<Point> projected =
        polygon3_to_2d(polygon, system);
    const Point projected_line_a = to_plane_coordinates(system, line.a);
    const Point projected_line_b = to_plane_coordinates(system, line.b);
    const Point projected_direction =
        projected_line_b - projected_line_a;
    const long double projected_length = abs(projected_direction);
    if(geometry_sign(projected_length) == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate projected 3D line");
    }
    const Point projected_unit = projected_direction / projected_length;
    std::vector<int> sides(projected.size());
    for(std::size_t index = 0; index < projected.size(); ++index){
        sides[index] = geometry_sign(cross(
            projected_unit, projected[index] - projected_line_a
        ));
    }

    std::vector<long double> events;
    std::vector<long double> boundary_events;
    std::vector<long double> toggle_events;
    std::vector<std::pair<long double, long double>> boundary_ranges;
    events.reserve(polygon.size() * 2 + 2);
    boundary_events.reserve(polygon.size() * 2);
    toggle_events.reserve(polygon.size());
    boundary_ranges.reserve(polygon.size());
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const std::size_t next = (index + 1) % polygon.size();
        const Segment3 edge{polygon[index], polygon[next]};
        const LinearIntersection3 intersection =
            line_segment_intersection(line, edge);
        std::optional<long double> point_parameter;
        if(const auto* point = std::get_if<Point3>(&intersection)){
            point_parameter = parameter(line, direction, *point);
            events.push_back(*point_parameter);
            boundary_events.push_back(*point_parameter);
        }else if(const auto* segment = std::get_if<Segment3>(&intersection)){
            long double first = parameter(line, direction, segment->a);
            long double second = parameter(line, direction, segment->b);
            if(second < first) std::swap(first, second);
            events.push_back(first);
            events.push_back(second);
            boundary_events.push_back(first);
            boundary_events.push_back(second);
            boundary_ranges.push_back({first, second});
        }

        if((sides[index] > 0) != (sides[next] > 0)){
            long double value = 0.0L;
            if(sides[index] == 0){
                value = parameter(line, direction, polygon[index]);
            }else if(sides[next] == 0){
                value = parameter(line, direction, polygon[next]);
            }else if(point_parameter){
                value = *point_parameter;
            }else[[unlikely]]{
                throw std::logic_error(
                    "polygon3 line crossing has no intersection"
                );
            }
            events.push_back(value);
            toggle_events.push_back(value);
        }
    }
    if(std::isfinite(lower)) events.push_back(lower);
    if(std::isfinite(upper)) events.push_back(upper);
    std::sort(events.begin(), events.end());
    events.erase(std::unique(events.begin(), events.end(), near), events.end());
    if(events.empty()) return result;

    std::vector<unsigned> toggle_count(events.size(), 0);
    std::vector<bool> boundary_event(events.size(), false);
    for(long double value: toggle_events){
        ++toggle_count[event_index(events, value)];
    }
    for(long double value: boundary_events){
        boundary_event[event_index(events, value)] = true;
    }
    std::sort(boundary_ranges.begin(), boundary_ranges.end());
    std::vector<std::pair<long double, long double>> merged_ranges;
    for(const auto& range: boundary_ranges){
        if(merged_ranges.empty()
            || (range.first > merged_ranges.back().second
                && !near(range.first, merged_ranges.back().second))){
            merged_ranges.push_back(range);
        }else{
            merged_ranges.back().second = std::max(
                merged_ranges.back().second, range.second
            );
        }
    }

    std::vector<bool> included_event = boundary_event;
    std::vector<bool> covered_event(events.size(), false);
    std::size_t range_index = 0;
    bool inside = false;
    for(std::size_t index = 0; index < events.size(); ++index){
        inside = inside != (toggle_count[index] % 2 != 0);
        if(index + 1 == events.size()) continue;

        const long double raw_middle =
            events[index] / 2.0L + events[index + 1] / 2.0L;
        while(range_index < merged_ranges.size()
            && merged_ranges[range_index].second < raw_middle
            && !near(merged_ranges[range_index].second, raw_middle)){
            ++range_index;
        }
        const bool on_boundary = range_index < merged_ranges.size()
            && (merged_ranges[range_index].first < raw_middle
                || near(merged_ranges[range_index].first, raw_middle))
            && (raw_middle < merged_ranges[range_index].second
                || near(raw_middle, merged_ranges[range_index].second));
        if(!inside && !on_boundary) continue;
        included_event[index] = true;
        included_event[index + 1] = true;

        const long double begin = std::max(events[index], lower);
        const long double end = std::min(events[index + 1], upper);
        if(begin > end || near(begin, end)) continue;
        const Segment3 segment{
            at(line, direction, begin),
            at(line, direction, end)
        };
        if(!result.segments.empty()
            && result.segments.back().b == segment.a){
            result.segments.back().b = segment.b;
        }else{
            result.segments.push_back(segment);
        }
        covered_event[index] = true;
        covered_event[index + 1] = true;
    }
    for(std::size_t index = 0; index < events.size(); ++index){
        if(events[index] < lower || events[index] > upper
            || !included_event[index] || covered_event[index]){
            continue;
        }
        result.points.push_back(at(line, direction, events[index]));
    }
    return result;
}

}  // namespace polygon3_line_intersection_detail

inline Polygon3LinearIntersection3 polygon3_line_intersection(
    const Polygon3& polygon,
    const Line3& line
){
    return polygon3_line_intersection_detail::intersect(
        polygon, line, -std::numeric_limits<long double>::infinity(),
        std::numeric_limits<long double>::infinity()
    );
}
