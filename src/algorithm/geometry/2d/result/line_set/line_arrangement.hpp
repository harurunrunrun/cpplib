#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_LINE_SET_LINE_ARRANGEMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_LINE_SET_LINE_ARRANGEMENT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../type/definition/line.hpp"
#include "../../type/definition/line_arrangement_result.hpp"

inline LineArrangementResult line_arrangement(const std::vector<Line>& lines){
    const std::size_t count = lines.size();
    std::vector<std::vector<std::pair<long double, Point>>> on_line(count);
    for(std::size_t index = 0; index < count; ++index){
        plus_geometry_detail::validate_finite(lines[index].a);
        plus_geometry_detail::validate_finite(lines[index].b);
        if(plus_geometry_detail::close(lines[index].a, lines[index].b)){
            throw std::invalid_argument("an arrangement line requires two distinct points");
        }
    }
    for(std::size_t first = 0; first < count; ++first){
        const Point first_direction = lines[first].b - lines[first].a;
        for(std::size_t second = first + 1; second < count; ++second){
            const Point second_direction = lines[second].b - lines[second].a;
            const long double denominator =
                cross(first_direction, second_direction);
            if(plus_geometry_detail::sign(
                denominator,
                std::abs(first_direction.x * second_direction.y)
                    + std::abs(first_direction.y * second_direction.x)
            ) == 0){
                continue;
            }
            const Point offset = lines[second].a - lines[first].a;
            const long double first_parameter =
                cross(offset, second_direction) / denominator;
            const long double second_parameter =
                cross(offset, first_direction) / denominator;
            const Point point = lines[first].a
                + first_direction * first_parameter;
            on_line[first].push_back({first_parameter, point});
            on_line[second].push_back({second_parameter, point});
        }
    }

    LineArrangementResult result;
    std::vector<Point> intersections;
    for(const auto& records: on_line){
        for(const auto& [parameter, point]: records){
            static_cast<void>(parameter);
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
        throw std::logic_error("line-arrangement vertex lookup failed");
    };
    std::vector<std::vector<std::size_t>> vertex_on_line(count);
    for(std::size_t line = 0; line < count; ++line){
        auto& points = on_line[line];
        std::sort(points.begin(), points.end(), [](const auto& left, const auto& right){
            return left.first < right.first;
        });
        for(const auto& [parameter, point]: points){
            static_cast<void>(parameter);
            const std::size_t id = vertex_id(point);
            if(vertex_on_line[line].empty()
                || vertex_on_line[line].back() != id){
                vertex_on_line[line].push_back(id);
            }
        }
        for(std::size_t index = 1; index < vertex_on_line[line].size(); ++index){
            result.bounded_edges.push_back({
                vertex_on_line[line][index - 1],
                vertex_on_line[line][index],
                line,
            });
        }
        const Point direction = lines[line].b - lines[line].a;
        const Point unit_direction =
            direction / std::sqrt(dot(direction, direction));
        if(vertex_on_line[line].empty()){
            result.unbounded_lines.push_back({
                lines[line].a, unit_direction, line
            });
        }else{
            result.rays.push_back({
                vertex_on_line[line].front(), -unit_direction, line
            });
            result.rays.push_back({
                vertex_on_line[line].back(), unit_direction, line
            });
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_LINE_SET_LINE_ARRANGEMENT_HPP_INCLUDED
