#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <variant>
#include <vector>

#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line3_direction.hpp"
#include "line_plane_intersection.hpp"
#include "line_segment_intersection.hpp"
#include "on_plane.hpp"
#include "polygon3_coplanar.hpp"
#include "polygon3_contains.hpp"
#include "polygon3_normal.hpp"

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

inline Polygon3LinearIntersection3 intersect(
    const Polygon3& polygon,
    const Line3& line,
    long double lower,
    long double upper
){
    if(polygon.size() < 3 || !polygon3_coplanar(polygon))[[unlikely]]{
        throw std::invalid_argument("polygon3 linear intersection requires a planar polygon");
    }
    const Point3 direction = line3_direction(line);
    const Plane3 plane{polygon.front(), polygon3_normal(polygon)};
    Polygon3LinearIntersection3 result;
    if(geometry3d_sign(dot(direction, plane.normal)) != 0){
        const Point3 point = line_plane_intersection(line, plane);
        const long double value = parameter(line, direction, point);
        if(value >= lower && value <= upper && polygon3_contains(polygon, point) != 0){
            result.points.push_back(point);
        }
        return result;
    }
    if(!on_plane(plane, line.a)) return result;

    std::vector<long double> events;
    events.reserve(polygon.size() * 2 + 2);
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Segment3 edge{polygon[index], polygon[(index + 1) % polygon.size()]};
        const LinearIntersection3 intersection = line_segment_intersection(line, edge);
        if(const auto* point = std::get_if<Point3>(&intersection)){
            events.push_back(parameter(line, direction, *point));
        }else if(const auto* segment = std::get_if<Segment3>(&intersection)){
            events.push_back(parameter(line, direction, segment->a));
            events.push_back(parameter(line, direction, segment->b));
        }
    }
    if(std::isfinite(lower)) events.push_back(lower);
    if(std::isfinite(upper)) events.push_back(upper);
    std::sort(events.begin(), events.end());
    events.erase(std::unique(events.begin(), events.end(), near), events.end());
    if(events.empty()) return result;

    for(std::size_t index = 0; index + 1 < events.size(); ++index){
        const long double begin = std::max(events[index], lower);
        const long double end = std::min(events[index + 1], upper);
        if(begin > end || near(begin, end)) continue;
        const long double middle = begin / 2.0L + end / 2.0L;
        if(polygon3_contains(polygon, at(line, direction, middle)) == 0) continue;
        const Segment3 segment{at(line, direction, begin), at(line, direction, end)};
        if(!result.segments.empty() && result.segments.back().b == segment.a){
            result.segments.back().b = segment.b;
        }else result.segments.push_back(segment);
    }
    for(long double event: events){
        if(event < lower || event > upper) continue;
        const Point3 point = at(line, direction, event);
        if(polygon3_contains(polygon, point) == 0) continue;
        bool covered = false;
        for(const Segment3& segment: result.segments){
            const long double first = parameter(line, direction, segment.a);
            const long double second = parameter(line, direction, segment.b);
            if(event >= std::min(first, second) && event <= std::max(first, second)){
                covered = true;
                break;
            }
        }
        if(!covered) result.points.push_back(point);
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
