#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "base.hpp"

namespace advanced_geometry_detail{

inline long double length(const Point& vector){
    return std::hypot(vector.x, vector.y);
}

inline long double scaled_tolerance(long double scale){
    return GEOMETRY_EPS * std::max(1.0L, scale);
}

inline int scaled_sign(long double value, long double scale){
    const long double tolerance = scaled_tolerance(scale);
    return (value > tolerance) - (value < -tolerance);
}

inline int cross_sign(const Point& first, const Point& second){
    return scaled_sign(
        cross(first, second),
        length(first) * length(second)
    );
}

inline long double point_tolerance(const Point& first, const Point& second){
    const long double scale = std::max({
        1.0L,
        std::abs(first.x),
        std::abs(first.y),
        std::abs(second.x),
        std::abs(second.y),
    });
    return GEOMETRY_EPS
        + 64.0L * std::numeric_limits<long double>::epsilon() * scale;
}

inline bool point_equal(const Point& first, const Point& second){
    const long double tolerance = point_tolerance(first, second);
    return std::abs(first.x - second.x) <= tolerance
        && std::abs(first.y - second.y) <= tolerance;
}

inline long double side_value(const Line& half_plane, const Point& point){
    return cross(half_plane.b - half_plane.a, point - half_plane.a);
}

inline int side_sign(const Line& half_plane, const Point& point){
    const Point direction = half_plane.b - half_plane.a;
    const Point relative = point - half_plane.a;
    const long double value = side_value(half_plane, point);
    const long double roundoff = 64.0L * std::numeric_limits<long double>::epsilon()
        * (std::abs(direction.x * relative.y)
           + std::abs(direction.y * relative.x));
    const long double tolerance = GEOMETRY_EPS * std::max(1.0L, length(direction))
        + roundoff;
    return (value > tolerance) - (value < -tolerance);
}

inline bool parallel_direction(const Point& first, const Point& second){
    return cross_sign(first, second) == 0;
}

inline bool same_direction(const Point& first, const Point& second){
    return parallel_direction(first, second) && dot(first, second) > 0.0L;
}

inline bool direction_less(const Point& first, const Point& second){
    const auto lower_half = [](const Point& direction){
        return direction.y < 0.0L
            || (direction.y == 0.0L && direction.x < 0.0L);
    };
    const bool first_lower = lower_half(first);
    const bool second_lower = lower_half(second);
    if(first_lower != second_lower) return !first_lower;
    return cross(first, second) > 0.0L;
}

inline Point line_intersection_unchecked(const Line& first, const Line& second){
    const Point first_direction = first.b - first.a;
    const Point second_direction = second.b - second.a;
    const long double denominator = cross(first_direction, second_direction);
    return first.a + first_direction
        * (cross(second.a - first.a, second_direction) / denominator);
}

inline void canonicalize_start(std::vector<Point>& polygon){
    if(polygon.size() == 2 && polygon[1] < polygon[0]){
        std::swap(polygon[0], polygon[1]);
        return;
    }
    if(polygon.size() < 3) return;
    const auto start = std::min_element(polygon.begin(), polygon.end());
    std::rotate(polygon.begin(), start, polygon.end());
}

inline void rotate_to_lowest(std::vector<Point>& polygon){
    if(polygon.size() < 2) return;
    const auto start = std::min_element(
        polygon.begin(),
        polygon.end(),
        [](const Point& first, const Point& second){
            if(first.y != second.y) return first.y < second.y;
            return first.x < second.x;
        }
    );
    std::rotate(polygon.begin(), start, polygon.end());
}

inline long double direction_angle(const Point& direction){
    long double angle = std::atan2(direction.y, direction.x);
    if(angle < 0.0L) angle += 2.0L * std::acos(-1.0L);
    return angle;
}

inline bool on_segment_scaled(const Segment& segment, const Point& point){
    const Point direction = segment.b - segment.a;
    if(point_equal(segment.a, segment.b)) return point_equal(segment.a, point);
    if(cross_sign(direction, point - segment.a) != 0) return false;
    const long double product = dot(point - segment.a, point - segment.b);
    const long double scale = length(point - segment.a) * length(point - segment.b);
    return scaled_sign(product, scale) <= 0;
}

inline std::vector<Point> segment_intersection_set(
    const Segment& first,
    const Segment& second
){
    const Point first_direction = first.b - first.a;
    const Point second_direction = second.b - second.a;
    if(point_equal(first.a, first.b)){
        return on_segment_scaled(second, first.a) ? std::vector<Point>{first.a}
                                                  : std::vector<Point>{};
    }
    if(point_equal(second.a, second.b)){
        return on_segment_scaled(first, second.a) ? std::vector<Point>{second.a}
                                                  : std::vector<Point>{};
    }
    if(cross_sign(first_direction, second_direction) != 0){
        const Point intersection = line_intersection_unchecked(first, second);
        if(on_segment_scaled(first, intersection) && on_segment_scaled(second, intersection)){
            return {intersection};
        }
        return {};
    }
    if(cross_sign(first_direction, second.a - first.a) != 0) return {};

    std::vector<Point> candidates;
    for(const Point& point: {first.a, first.b, second.a, second.b}){
        if(on_segment_scaled(first, point) && on_segment_scaled(second, point)
           && std::none_of(candidates.begin(), candidates.end(), [&](const Point& other){
               return point_equal(point, other);
           })){
            candidates.push_back(point);
        }
    }
    if(candidates.empty()) return {};
    const auto [minimum, maximum] = std::minmax_element(candidates.begin(), candidates.end());
    if(point_equal(*minimum, *maximum)) return {*minimum};
    return {*minimum, *maximum};
}

inline bool point_in_normalized_convex_polygon(
    const std::vector<Point>& polygon,
    const Point& point
){
    if(polygon.empty()) return false;
    if(polygon.size() == 1) return point_equal(polygon.front(), point);
    if(polygon.size() == 2) return on_segment_scaled({polygon[0], polygon[1]}, point);
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Line edge{polygon[index], polygon[(index + 1) % polygon.size()]};
        if(side_sign(edge, point) < 0) return false;
    }
    return true;
}

inline std::vector<Point> clip_segment_by_convex_polygon(
    const Segment& segment,
    const std::vector<Point>& polygon
){
    Point direction = segment.b - segment.a;
    long double lower = 0.0L;
    long double upper = 1.0L;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Line edge{polygon[index], polygon[(index + 1) % polygon.size()]};
        const long double constant = side_value(edge, segment.a);
        const long double coefficient = cross(edge.b - edge.a, direction);
        const long double coefficient_scale = length(edge.b - edge.a) * length(direction);
        if(scaled_sign(coefficient, coefficient_scale) == 0){
            if(side_sign(edge, segment.a) < 0) return {};
            continue;
        }
        const long double boundary = -constant / coefficient;
        if(coefficient > 0.0L) lower = std::max(lower, boundary);
        else upper = std::min(upper, boundary);
        if(lower > upper + GEOMETRY_EPS) return {};
    }
    lower = std::clamp(lower, 0.0L, 1.0L);
    upper = std::clamp(upper, 0.0L, 1.0L);
    if(lower > upper + GEOMETRY_EPS) return {};
    Point first = segment.a + direction * lower;
    Point second = segment.a + direction * upper;
    if(point_equal(first, second)) return {first};
    std::vector<Point> result = {first, second};
    canonicalize_start(result);
    return result;
}

}  // namespace advanced_geometry_detail
