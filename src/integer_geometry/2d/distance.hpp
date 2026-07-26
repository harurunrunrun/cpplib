#ifndef CPPLIB_SRC_INTEGER_GEOMETRY_2D_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_INTEGER_GEOMETRY_2D_DISTANCE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

#include "convex.hpp"

namespace integer_geometry{

inline Rational squared_norm(const Vector& vector){
    return dot(vector, vector);
}

inline Rational squared_distance(const Point& first, const Point& second){
    return squared_norm(first - second);
}

namespace detail{

inline bool by_y_then_x(const Point& first, const Point& second){
    return first.y < second.y
        || (first.y == second.y && first.x < second.x);
}

inline void minimize(
    std::optional<Rational>& target,
    const std::optional<Rational>& candidate
){
    if(candidate && (!target || *candidate < *target)) target = candidate;
}

inline std::optional<Rational> closest_pair_recursive(
    std::vector<Point>& points,
    std::vector<Point>& buffer,
    std::size_t left,
    std::size_t right
){
    if(right - left <= 3){
        std::optional<Rational> best;
        for(std::size_t first = left; first < right; ++first){
            for(std::size_t second = first + 1; second < right; ++second){
                minimize(
                    best,
                    squared_distance(points[first], points[second])
                );
            }
        }
        std::sort(
            points.begin() + static_cast<std::ptrdiff_t>(left),
            points.begin() + static_cast<std::ptrdiff_t>(right),
            by_y_then_x
        );
        return best;
    }

    const std::size_t middle = (left + right) / 2;
    const Rational middle_x = points[middle].x;
    std::optional<Rational> best =
        closest_pair_recursive(points, buffer, left, middle);
    minimize(
        best,
        closest_pair_recursive(points, buffer, middle, right)
    );
    std::merge(
        points.begin() + static_cast<std::ptrdiff_t>(left),
        points.begin() + static_cast<std::ptrdiff_t>(middle),
        points.begin() + static_cast<std::ptrdiff_t>(middle),
        points.begin() + static_cast<std::ptrdiff_t>(right),
        buffer.begin() + static_cast<std::ptrdiff_t>(left),
        by_y_then_x
    );
    std::copy(
        buffer.begin() + static_cast<std::ptrdiff_t>(left),
        buffer.begin() + static_cast<std::ptrdiff_t>(right),
        points.begin() + static_cast<std::ptrdiff_t>(left)
    );

    std::vector<Point> strip;
    strip.reserve(right - left);
    for(std::size_t index = left; index < right; ++index){
        const Rational x_difference = points[index].x - middle_x;
        if(best && x_difference * x_difference >= *best) continue;
        for(std::size_t previous = strip.size(); previous-- > 0;){
            const Rational y_difference =
                points[index].y - strip[previous].y;
            if(best && y_difference * y_difference >= *best) break;
            minimize(
                best,
                squared_distance(points[index], strip[previous])
            );
        }
        strip.push_back(points[index]);
    }
    return best;
}

}  // namespace detail

inline std::optional<Rational> closest_pair_squared(
    std::vector<Point> points
){
    if(points.size() < 2) return std::nullopt;
    std::sort(points.begin(), points.end());
    std::vector<Point> buffer(points.size());
    return detail::closest_pair_recursive(
        points, buffer, 0, points.size()
    );
}

inline std::optional<Rational> convex_diameter_squared(
    const std::vector<Point>& convex_polygon
){
    const std::size_t size = convex_polygon.size();
    if(size == 0) return std::nullopt;
    if(size == 1) return Rational{0};
    if(size == 2){
        return squared_distance(convex_polygon[0], convex_polygon[1]);
    }

    bool all_collinear = true;
    for(std::size_t index = 2; index < size; ++index){
        if(orientation(
            convex_polygon[0], convex_polygon[1], convex_polygon[index]
        ) != 0){
            all_collinear = false;
            break;
        }
    }
    if(all_collinear){
        const auto endpoints = std::minmax_element(
            convex_polygon.begin(), convex_polygon.end()
        );
        return squared_distance(*endpoints.first, *endpoints.second);
    }

    const auto doubled_area = [&](
        const Vector& edge,
        std::size_t edge_start,
        std::size_t vertex
    ){
        return abs(cross(
            edge,
            convex_polygon[vertex] - convex_polygon[edge_start]
        ));
    };

    std::size_t opposite = 1;
    const Vector first_edge = convex_polygon[1] - convex_polygon[0];
    Rational opposite_area = doubled_area(first_edge, 0, opposite);
    for(std::size_t candidate = 2; candidate < size; ++candidate){
        const Rational candidate_area =
            doubled_area(first_edge, 0, candidate);
        if(opposite_area <= candidate_area){
            opposite = candidate;
            opposite_area = candidate_area;
        }
    }

    Rational best = 0;
    for(std::size_t index = 0; index < size; ++index){
        const std::size_t next = (index + 1) % size;
        const Vector edge = convex_polygon[next] - convex_polygon[index];
        opposite_area = doubled_area(edge, index, opposite);
        while(true){
            const std::size_t next_opposite = (opposite + 1) % size;
            const Rational next_area =
                doubled_area(edge, index, next_opposite);
            if(next_area <= opposite_area) break;
            opposite = next_opposite;
            opposite_area = next_area;
        }
        best = std::max(
            best,
            squared_distance(convex_polygon[index], convex_polygon[opposite])
        );
        best = std::max(
            best,
            squared_distance(convex_polygon[next], convex_polygon[opposite])
        );
        const std::size_t next_opposite = (opposite + 1) % size;
        if(doubled_area(edge, index, next_opposite) == opposite_area){
            best = std::max(
                best,
                squared_distance(
                    convex_polygon[index],
                    convex_polygon[next_opposite]
                )
            );
            best = std::max(
                best,
                squared_distance(
                    convex_polygon[next],
                    convex_polygon[next_opposite]
                )
            );
        }
    }
    return best;
}

inline std::optional<Rational> point_set_diameter_squared(
    std::vector<Point> points
){
    if(points.empty()) return std::nullopt;
    return convex_diameter_squared(convex_hull(std::move(points)));
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_INTEGER_GEOMETRY_2D_DISTANCE_HPP_INCLUDED
