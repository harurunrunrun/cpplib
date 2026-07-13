#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "geometry.hpp"

enum HalfPlaneIntersectionKind{
    HALF_PLANE_EMPTY,
    HALF_PLANE_BOUNDED,
    HALF_PLANE_UNBOUNDED,
};

struct HalfPlaneIntersectionResult{
    HalfPlaneIntersectionKind kind = HALF_PLANE_EMPTY;
    std::vector<Point> polygon;
};

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

}  // namespace advanced_geometry_detail

inline bool half_plane_contains(const Line& half_plane, const Point& point){
    const Point direction = half_plane.b - half_plane.a;
    if(advanced_geometry_detail::length(direction) <= GEOMETRY_EPS){
        throw std::invalid_argument("a half-plane boundary requires two distinct points");
    }
    return advanced_geometry_detail::side_sign(half_plane, point) >= 0;
}

inline std::vector<Point> normalize_convex_polygon(std::vector<Point> polygon){
    using namespace advanced_geometry_detail;

    std::vector<Point> vertices;
    vertices.reserve(polygon.size());
    for(const Point& point: polygon){
        if(vertices.empty() || !point_equal(vertices.back(), point)){
            vertices.push_back(point);
        }
    }
    if(vertices.size() >= 2 && point_equal(vertices.front(), vertices.back())){
        vertices.pop_back();
    }
    if(vertices.size() <= 1) return vertices;

    const int count = static_cast<int>(vertices.size());
    int orientation = 0;
    for(int index = 0; index < count; ++index){
        const int turn = cross_sign(
            vertices[static_cast<std::size_t>((index + 1) % count)]
                - vertices[static_cast<std::size_t>(index)],
            vertices[static_cast<std::size_t>((index + 2) % count)]
                - vertices[static_cast<std::size_t>((index + 1) % count)]
        );
        if(turn == 0) continue;
        if(orientation == 0) orientation = turn;
        else if(orientation != turn){
            throw std::invalid_argument("the polygon is not convex");
        }
    }

    if(orientation == 0){
        const auto [minimum, maximum] = std::minmax_element(vertices.begin(), vertices.end());
        if(point_equal(*minimum, *maximum)) return {*minimum};
        return {*minimum, *maximum};
    }
    if(orientation < 0) std::reverse(vertices.begin(), vertices.end());

    int start = 0;
    for(int index = 0; index < count; ++index){
        if(cross_sign(
            vertices[static_cast<std::size_t>((index + 1) % count)]
                - vertices[static_cast<std::size_t>(index)],
            vertices[static_cast<std::size_t>((index + 2) % count)]
                - vertices[static_cast<std::size_t>((index + 1) % count)]
        ) > 0){
            start = index;
            break;
        }
    }

    std::vector<Point> result;
    result.reserve(vertices.size() + 1);
    for(int offset = 0; offset <= count; ++offset){
        const Point& point = vertices[static_cast<std::size_t>((start + offset) % count)];
        if(!result.empty() && point_equal(result.back(), point)) continue;
        while(result.size() >= 2 && cross_sign(
            result.back() - result[result.size() - 2],
            point - result.back()
        ) == 0){
            result.pop_back();
        }
        result.push_back(point);
    }
    if(!result.empty()) result.pop_back();
    canonicalize_start(result);
    return result;
}

inline std::vector<Point> convex_polygon_minkowski_sum(
    std::vector<Point> first,
    std::vector<Point> second
){
    using namespace advanced_geometry_detail;

    first = normalize_convex_polygon(std::move(first));
    second = normalize_convex_polygon(std::move(second));
    if(first.empty() || second.empty()) return {};
    if(first.size() == 1){
        for(Point& point: second) point += first.front();
        return normalize_convex_polygon(std::move(second));
    }
    if(second.size() == 1){
        for(Point& point: first) point += second.front();
        return normalize_convex_polygon(std::move(first));
    }

    rotate_to_lowest(first);
    rotate_to_lowest(second);
    std::vector<Point> first_edges(first.size());
    std::vector<Point> second_edges(second.size());
    for(std::size_t index = 0; index < first.size(); ++index){
        first_edges[index] = first[(index + 1) % first.size()] - first[index];
    }
    for(std::size_t index = 0; index < second.size(); ++index){
        second_edges[index] = second[(index + 1) % second.size()] - second[index];
    }

    std::vector<Point> result;
    result.reserve(first.size() + second.size() + 1);
    result.push_back(first.front() + second.front());
    std::size_t first_index = 0;
    std::size_t second_index = 0;
    while(first_index < first_edges.size() || second_index < second_edges.size()){
        Point step;
        if(first_index == first_edges.size()){
            step = second_edges[second_index++];
        }else if(second_index == second_edges.size()){
            step = first_edges[first_index++];
        }else if(same_direction(first_edges[first_index], second_edges[second_index])){
            step = first_edges[first_index++] + second_edges[second_index++];
        }else if(direction_less(
            first_edges[first_index],
            second_edges[second_index]
        )){
            step = first_edges[first_index++];
        }else{
            step = second_edges[second_index++];
        }
        result.push_back(result.back() + step);
    }
    if(result.size() >= 2 && point_equal(result.front(), result.back())) result.pop_back();
    return normalize_convex_polygon(std::move(result));
}

inline HalfPlaneIntersectionResult half_plane_intersection(
    std::vector<Line> half_planes
){
    using namespace advanced_geometry_detail;

    struct TaggedHalfPlane{
        Line line;
        long double angle;
        bool artificial;
    };

    long double coordinate_bound = 1.0L;
    std::vector<TaggedHalfPlane> sorted;
    sorted.reserve(half_planes.size() + 4);
    for(const Line& half_plane: half_planes){
        const Point direction = half_plane.b - half_plane.a;
        if(length(direction) <= GEOMETRY_EPS){
            throw std::invalid_argument("a half-plane boundary requires two distinct points");
        }
        for(long double coordinate: {
            half_plane.a.x,
            half_plane.a.y,
            half_plane.b.x,
            half_plane.b.y,
        }){
            if(!std::isfinite(coordinate)){
                throw std::invalid_argument("half-plane coordinates must be finite");
            }
            coordinate_bound = std::max(coordinate_bound, std::abs(coordinate));
        }
        sorted.push_back({half_plane, direction_angle(direction), false});
    }

    const long double box_factor = 128.0L / GEOMETRY_EPS;
    const long double safe_limit = std::sqrt(std::numeric_limits<long double>::max())
        / box_factor;
    if(coordinate_bound > safe_limit){
        throw std::overflow_error("half-plane coordinates are too large for adaptive bounding");
    }
    const long double box = (coordinate_bound + 1.0L) * box_factor;
    const std::vector<Line> bounding_box = {
        {{-box, -box}, {box, -box}},
        {{box, -box}, {box, box}},
        {{box, box}, {-box, box}},
        {{-box, box}, {-box, -box}},
    };
    for(const Line& half_plane: bounding_box){
        sorted.push_back({
            half_plane,
            direction_angle(half_plane.b - half_plane.a),
            true,
        });
    }

    std::sort(
        sorted.begin(),
        sorted.end(),
        [](const TaggedHalfPlane& first, const TaggedHalfPlane& second){
            if(first.angle != second.angle) return first.angle < second.angle;
            const Point first_direction = first.line.b - first.line.a;
            const Point second_direction = second.line.b - second.line.a;
            const long double first_offset = cross(
                first_direction / advanced_geometry_detail::length(first_direction),
                first.line.a
            );
            const long double second_offset = cross(
                second_direction / advanced_geometry_detail::length(second_direction),
                second.line.a
            );
            return first_offset > second_offset;
        }
    );

    auto more_restrictive = [](const TaggedHalfPlane& first, const TaggedHalfPlane& second){
        const Point first_direction = first.line.b - first.line.a;
        const Point second_direction = second.line.b - second.line.a;
        const long double first_offset = cross(
            first_direction / advanced_geometry_detail::length(first_direction),
            first.line.a
        );
        const long double second_offset = cross(
            second_direction / advanced_geometry_detail::length(second_direction),
            second.line.a
        );
        return second_offset > first_offset ? second : first;
    };

    std::vector<TaggedHalfPlane> unique;
    unique.reserve(sorted.size());
    for(const TaggedHalfPlane& half_plane: sorted){
        if(!unique.empty() && same_direction(
            unique.back().line.b - unique.back().line.a,
            half_plane.line.b - half_plane.line.a
        )){
            unique.back() = more_restrictive(unique.back(), half_plane);
        }else{
            unique.push_back(half_plane);
        }
    }
    if(unique.size() >= 2 && same_direction(
        unique.front().line.b - unique.front().line.a,
        unique.back().line.b - unique.back().line.a
    )){
        unique.front() = more_restrictive(unique.front(), unique.back());
        unique.pop_back();
    }

    auto outside = [](const TaggedHalfPlane& half_plane, const Point& point){
        return advanced_geometry_detail::side_sign(half_plane.line, point) < 0;
    };
    auto adjacent_intersection = [](const TaggedHalfPlane& first, const TaggedHalfPlane& second){
        return advanced_geometry_detail::line_intersection_unchecked(first.line, second.line);
    };

    std::deque<TaggedHalfPlane> deque;
    for(const TaggedHalfPlane& half_plane: unique){
        while(deque.size() >= 2){
            const Point intersection = adjacent_intersection(
                deque[deque.size() - 2],
                deque.back()
            );
            if(!outside(half_plane, intersection)) break;
            deque.pop_back();
        }
        while(deque.size() >= 2){
            const Point intersection = adjacent_intersection(deque[0], deque[1]);
            if(!outside(half_plane, intersection)) break;
            deque.pop_front();
        }
        if(!deque.empty() && parallel_direction(
            deque.back().line.b - deque.back().line.a,
            half_plane.line.b - half_plane.line.a
        )){
            if(same_direction(
                deque.back().line.b - deque.back().line.a,
                half_plane.line.b - half_plane.line.a
            )){
                deque.back() = more_restrictive(deque.back(), half_plane);
                continue;
            }
            return {HALF_PLANE_EMPTY, {}};
        }
        deque.push_back(half_plane);
    }

    while(deque.size() >= 3){
        const Point intersection = adjacent_intersection(
            deque[deque.size() - 2],
            deque.back()
        );
        if(!outside(deque.front(), intersection)) break;
        deque.pop_back();
    }
    while(deque.size() >= 3){
        const Point intersection = adjacent_intersection(deque[0], deque[1]);
        if(!outside(deque.back(), intersection)) break;
        deque.pop_front();
    }
    if(deque.size() < 3) return {HALF_PLANE_EMPTY, {}};
    if(parallel_direction(
        deque.front().line.b - deque.front().line.a,
        deque.back().line.b - deque.back().line.a
    )){
        return {HALF_PLANE_EMPTY, {}};
    }

    std::vector<Point> polygon;
    polygon.reserve(deque.size());
    bool artificial_boundary = false;
    for(std::size_t index = 0; index < deque.size(); ++index){
        const TaggedHalfPlane& current = deque[index];
        const TaggedHalfPlane& next = deque[(index + 1) % deque.size()];
        if(parallel_direction(
            current.line.b - current.line.a,
            next.line.b - next.line.a
        )){
            return {HALF_PLANE_EMPTY, {}};
        }
        polygon.push_back(adjacent_intersection(current, next));
        artificial_boundary = artificial_boundary || current.artificial;
    }
    if(artificial_boundary) return {HALF_PLANE_UNBOUNDED, {}};

    polygon = normalize_convex_polygon(std::move(polygon));
    if(polygon.empty()) return {HALF_PLANE_EMPTY, {}};
    return {HALF_PLANE_BOUNDED, std::move(polygon)};
}

namespace advanced_geometry_detail{

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

    std::vector<Line> half_planes;
    half_planes.reserve(first.size() + second.size());
    for(std::size_t index = 0; index < first.size(); ++index){
        half_planes.push_back({first[index], first[(index + 1) % first.size()]});
    }
    for(std::size_t index = 0; index < second.size(); ++index){
        half_planes.push_back({second[index], second[(index + 1) % second.size()]});
    }
    HalfPlaneIntersectionResult result = half_plane_intersection(std::move(half_planes));
    if(result.kind == HALF_PLANE_EMPTY) return {};
    if(result.kind != HALF_PLANE_BOUNDED){
        throw std::runtime_error("intersection of two bounded convex polygons became unbounded");
    }
    return normalize_convex_polygon(std::move(result.polygon));
}
