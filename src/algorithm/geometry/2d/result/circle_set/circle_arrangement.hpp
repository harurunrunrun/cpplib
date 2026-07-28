#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_CIRCLE_SET_CIRCLE_ARRANGEMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_CIRCLE_SET_CIRCLE_ARRANGEMENT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <numbers>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../point_set/circle_circle/circle_circle_cross_points.hpp"
#include "../../type/definition/circle_arrangement_result.hpp"

inline CircleArrangementResult circle_arrangement(
    const std::vector<Circle>& circles
){
    const std::size_t count = circles.size();
    for(const Circle& circle: circles){
        plus_geometry_detail::validate_finite(circle.center);
        if(!std::isfinite(circle.radius) || circle.radius <= 0.0L){
            throw std::invalid_argument(
                "an arrangement circle requires a positive finite radius"
            );
        }
    }
    std::vector<std::vector<std::pair<long double, Point>>> on_circle(count);
    for(std::size_t first = 0; first < count; ++first){
        for(std::size_t second = first + 1; second < count; ++second){
            if(plus_geometry_detail::close(
                circles[first].center, circles[second].center
            ) && plus_geometry_detail::sign(
                circles[first].radius - circles[second].radius,
                std::max(circles[first].radius, circles[second].radius)
            ) == 0){
                throw std::invalid_argument(
                    "coincident circles do not define a finite arrangement"
                );
            }
            const std::vector<Point> intersections =
                circle_circle_cross_points(circles[first], circles[second]);
            for(const Point& point: intersections){
                const long double first_angle = std::atan2(
                    point.y - circles[first].center.y,
                    point.x - circles[first].center.x
                );
                const long double second_angle = std::atan2(
                    point.y - circles[second].center.y,
                    point.x - circles[second].center.x
                );
                on_circle[first].push_back({first_angle, point});
                on_circle[second].push_back({second_angle, point});
            }
        }
    }

    CircleArrangementResult result;
    std::vector<Point> intersections;
    for(const auto& records: on_circle){
        for(const auto& [angle, point]: records){
            static_cast<void>(angle);
            intersections.push_back(point);
        }
    }
    std::sort(intersections.begin(), intersections.end());
    for(const Point& point: intersections){
        if(result.vertices.empty()
            || !plus_geometry_detail::close(result.vertices.back(), point)){
            result.vertices.push_back(point);
        }
    }
    const auto vertex_id = [&](const Point& point){
        auto iterator = std::lower_bound(
            result.vertices.begin(), result.vertices.end(), point
        );
        if(iterator != result.vertices.end()
            && plus_geometry_detail::close(*iterator, point)){
            return static_cast<std::size_t>(
                iterator - result.vertices.begin()
            );
        }
        if(iterator != result.vertices.begin()){
            --iterator;
            if(plus_geometry_detail::close(*iterator, point)){
                return static_cast<std::size_t>(
                    iterator - result.vertices.begin()
                );
            }
        }
        throw std::logic_error("circle-arrangement vertex lookup failed");
    };
    constexpr std::size_t no_vertex = static_cast<std::size_t>(-1);
    for(std::size_t circle = 0; circle < count; ++circle){
        auto& points = on_circle[circle];
        std::sort(points.begin(), points.end(), [](const auto& left, const auto& right){
            return left.first < right.first;
        });
        std::map<std::size_t, long double> angle_by_vertex;
        for(const auto& [angle, point]: points){
            angle_by_vertex.try_emplace(vertex_id(point), angle);
        }
        std::vector<std::pair<long double, std::size_t>> ordered;
        for(const auto& [vertex, angle]: angle_by_vertex){
            ordered.push_back({angle, vertex});
        }
        std::sort(ordered.begin(), ordered.end());
        if(ordered.empty()){
            result.arcs.push_back({
                circle,
                no_vertex,
                no_vertex,
                -std::numbers::pi_v<long double>,
                std::numbers::pi_v<long double>,
                true,
            });
            continue;
        }
        for(std::size_t index = 0; index < ordered.size(); ++index){
            const std::size_t next = (index + 1) % ordered.size();
            long double end_angle = ordered[next].first;
            if(next == 0) end_angle += 2.0L * std::numbers::pi_v<long double>;
            result.arcs.push_back({
                circle,
                ordered[index].second,
                ordered[next].second,
                ordered[index].first,
                end_angle,
                false,
            });
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_CIRCLE_SET_CIRCLE_ARRANGEMENT_HPP_INCLUDED
