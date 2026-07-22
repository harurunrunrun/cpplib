#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_HALF_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_HALF_PLANE_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "normalize_convex_polygon.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_HALF_PLANE_INTERSECTION_HPP_INCLUDED
