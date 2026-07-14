#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "constants.hpp"
#include "contains.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "intersect.hpp"
#include "norm.hpp"
#include "types.hpp"

namespace visibility_polygon_detail{

inline bool finite_point(const Point& point){
    return std::isfinite(point.x) && std::isfinite(point.y);
}

inline void validate_simple_polygon(const std::vector<Point>& polygon){
    const std::size_t size = polygon.size();
    if(size < 3)[[unlikely]]{
        throw std::invalid_argument("polygon must have at least three vertices");
    }
    long double twice_area = 0.0L;
    for(std::size_t index = 0; index < size; ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % size];
        if(!finite_point(first))[[unlikely]]{
            throw std::invalid_argument("polygon vertex must be finite");
        }
        if(norm(second - first)
            <= GEOMETRY_EPS * GEOMETRY_EPS)[[unlikely]]{
            throw std::invalid_argument("polygon has a degenerate edge");
        }
        twice_area += cross(first, second);
    }
    if(std::fabs(twice_area) <= GEOMETRY_EPS)[[unlikely]]{
        throw std::invalid_argument("polygon area must be nonzero");
    }
    for(std::size_t first = 0; first < size; ++first){
        const Segment first_edge{
            polygon[first], polygon[(first + 1) % size]
        };
        for(std::size_t second = first + 1; second < size; ++second){
            if(second == first + 1
                || (first == 0 && second + 1 == size)){
                continue;
            }
            const Segment second_edge{
                polygon[second], polygon[(second + 1) % size]
            };
            if(intersect(first_edge, second_edge))[[unlikely]]{
                throw std::invalid_argument("polygon must be simple");
            }
        }
    }
}

inline std::pair<bool, long double> ray_segment_distance(
    const Point& origin,
    const Point& direction,
    const Segment& segment
){
    const Point edge = segment.b - segment.a;
    const Point offset = segment.a - origin;
    const long double denominator = cross(direction, edge);
    const long double scale = std::sqrt(
        std::max(0.0L, norm(direction) * norm(edge))
    );
    if(std::fabs(denominator)
        <= 1e-18L * std::max(1.0L, scale)){
        if(std::fabs(cross(offset, direction))
            > GEOMETRY_EPS * std::max(1.0L, std::sqrt(norm(offset)))){
            return {false, 0.0L};
        }
        const long double divisor = norm(direction);
        long double answer = std::numeric_limits<long double>::infinity();
        for(const Point& point: {segment.a, segment.b}){
            const long double value = dot(point - origin, direction) / divisor;
            if(value >= -GEOMETRY_EPS) answer = std::min(answer, value);
        }
        return {std::isfinite(answer), std::max(0.0L, answer)};
    }
    const long double distance = cross(offset, edge) / denominator;
    const long double position = cross(offset, direction) / denominator;
    if(distance < -GEOMETRY_EPS
        || position < -GEOMETRY_EPS
        || position > 1.0L + GEOMETRY_EPS){
        return {false, 0.0L};
    }
    return {true, std::max(0.0L, distance)};
}

inline Point nearest_ray_boundary_point(
    const std::vector<Point>& polygon,
    const Point& observer,
    long double angle
){
    const Point direction{std::cos(angle), std::sin(angle)};
    long double best = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const auto [hit, distance] = ray_segment_distance(
            observer,
            direction,
            {polygon[index], polygon[(index + 1) % polygon.size()]}
        );
        if(hit) best = std::min(best, distance);
    }
    if(!std::isfinite(best))[[unlikely]]{
        throw std::runtime_error("ray did not hit polygon boundary");
    }
    return observer + direction * best;
}

}

inline std::vector<Point> visibility_polygon(
    const std::vector<Point>& polygon,
    const Point& observer
){
    visibility_polygon_detail::validate_simple_polygon(polygon);
    if(!visibility_polygon_detail::finite_point(observer))[[unlikely]]{
        throw std::invalid_argument("observer must be finite");
    }
    if(contains(polygon, observer) != 2)[[unlikely]]{
        throw std::invalid_argument("observer must be strictly inside polygon");
    }

    const long double turn = 2.0L * std::acos(-1.0L);
    const auto normalize_angle = [turn](long double angle){
        angle = std::fmod(angle, turn);
        if(angle < 0.0L) angle += turn;
        if(angle >= turn) angle = 0.0L;
        return angle;
    };
    std::vector<long double> event_angles;
    event_angles.reserve(polygon.size());
    for(const Point& vertex: polygon){
        event_angles.push_back(normalize_angle(std::atan2(
            vertex.y - observer.y,
            vertex.x - observer.x
        )));
    }
    std::sort(event_angles.begin(), event_angles.end());
    event_angles.erase(
        std::unique(event_angles.begin(), event_angles.end()),
        event_angles.end()
    );

    std::vector<std::pair<long double, Point>> candidates;
    candidates.reserve(event_angles.size() * 3);
    for(std::size_t index = 0; index < event_angles.size(); ++index){
        const long double angle = event_angles[index];
        const long double previous = event_angles[
            (index + event_angles.size() - 1) % event_angles.size()
        ];
        const long double next = event_angles[
            (index + 1) % event_angles.size()
        ];
        const long double previous_gap = normalize_angle(angle - previous);
        const long double next_gap = normalize_angle(next - angle);
        const long double before_delta =
            std::min(1e-7L, previous_gap / 4.0L);
        const long double after_delta =
            std::min(1e-7L, next_gap / 4.0L);
        for(const long double current_angle: {
            normalize_angle(angle - before_delta),
            angle,
            normalize_angle(angle + after_delta)
        }){
            candidates.emplace_back(
                current_angle,
                visibility_polygon_detail::nearest_ray_boundary_point(
                    polygon, observer, current_angle
                )
            );
        }
    }
    std::sort(candidates.begin(), candidates.end(),
        [](const auto& first, const auto& second){
            return first.first < second.first;
        });

    long double coordinate_scale = 1.0L;
    for(const Point& point: polygon){
        coordinate_scale = std::max(
            coordinate_scale,
            std::max(std::fabs(point.x), std::fabs(point.y))
        );
    }
    const long double merge_distance =
        GEOMETRY_EPS * coordinate_scale * 8.0L;
    const long double merge_distance_squared =
        merge_distance * merge_distance;
    std::vector<Point> result;
    result.reserve(candidates.size());
    for(const auto& [angle, point]: candidates){
        static_cast<void>(angle);
        if(result.empty()
            || norm(point - result.back()) > merge_distance_squared){
            result.push_back(point);
        }
    }
    if(result.size() > 1
        && norm(result.front() - result.back()) <= merge_distance_squared){
        result.pop_back();
    }
    return result;
}
