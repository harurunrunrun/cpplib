#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_DISTANCE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

#include <utility>
#include "convex.hpp"

namespace integer_geometry{

namespace detail{

inline UnsignedWide squared_component_distance(
    Coordinate first,
    Coordinate second
){
    const UnsignedWide difference = first >= second
        ? static_cast<UnsignedWide>(static_cast<std::uint64_t>(first)
            - static_cast<std::uint64_t>(second))
        : static_cast<UnsignedWide>(static_cast<std::uint64_t>(second)
            - static_cast<std::uint64_t>(first));
    return difference * difference;
}

inline std::optional<UnsignedWide> representable_squared_distance(
    const Point& first,
    const Point& second
){
    const UnsignedWide x_square =
        squared_component_distance(first.x, second.x);
    const UnsignedWide y_square =
        squared_component_distance(first.y, second.y);
    constexpr UnsignedWide maximum = ~UnsignedWide{0};
    if(y_square > maximum - x_square) return std::nullopt;
    return x_square + y_square;
}
}  // namespace detail


inline UnsignedWide squared_distance(const Point& first, const Point& second){
    const auto result = detail::representable_squared_distance(first, second);
    if(!result.has_value()){
        throw std::overflow_error(
            "integer geometry squared distance overflow"
        );
    }
    return *result;
}

namespace detail{

inline bool by_y_then_x(const Point& first, const Point& second){
    return first.y < second.y
        || (first.y == second.y && first.x < second.x);
}

inline void minimize_optional(
    std::optional<UnsignedWide>& target,
    const std::optional<UnsignedWide>& candidate
){
    if(candidate.has_value()
        && (!target.has_value() || *candidate < *target)){
        target = candidate;
    }
}

inline std::optional<UnsignedWide> closest_pair_recursive(
    std::vector<Point>& points,
    std::vector<Point>& buffer,
    std::size_t left,
    std::size_t right
){
    if(right - left <= 3){
        std::optional<UnsignedWide> best;
        for(std::size_t first = left; first < right; ++first){
            for(std::size_t second = first + 1; second < right; ++second){
                minimize_optional(best, representable_squared_distance(
                    points[first], points[second]
                ));
            }
        }
        std::sort(points.begin() + static_cast<std::ptrdiff_t>(left),
                  points.begin() + static_cast<std::ptrdiff_t>(right),
                  by_y_then_x);
        return best;
    }

    const std::size_t middle = (left + right) / 2;
    const Coordinate middle_x = points[middle].x;
    std::optional<UnsignedWide> best = closest_pair_recursive(
        points, buffer, left, middle
    );
    minimize_optional(best, closest_pair_recursive(
        points, buffer, middle, right
    ));
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
        if(best.has_value()
            && squared_component_distance(points[index].x, middle_x) >= *best){
            continue;
        }
        for(std::size_t previous = strip.size(); previous-- > 0;){
            if(best.has_value() && squared_component_distance(
                points[index].y, strip[previous].y
            ) >= *best){
                break;
            }
            minimize_optional(best, representable_squared_distance(
                points[index], strip[previous]
            ));
        }
        strip.push_back(points[index]);
    }
    return best;
}

}  // namespace detail

inline std::optional<UnsignedWide> closest_pair_squared(
    std::vector<Point> points
){
    if(points.size() < 2) return std::nullopt;
    std::sort(points.begin(), points.end());
    std::vector<Point> buffer(points.size());
    const auto result =
        detail::closest_pair_recursive(points, buffer, 0, points.size());
    if(!result.has_value()){
        throw std::overflow_error(
            "integer geometry closest-pair squared distance overflow"
        );
    }
    return result;
}

inline std::optional<UnsignedWide> convex_diameter_squared(
    const std::vector<Point>& convex_polygon
){
    const std::size_t size = convex_polygon.size();
    if(size == 0) return std::nullopt;
    if(size == 1) return UnsignedWide{0};
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
        const auto [minimum, maximum] = std::minmax_element(
            convex_polygon.begin(), convex_polygon.end()
        );
        return squared_distance(*minimum, *maximum);
    }

    const auto doubled_area = [&](
        const Vector& edge,
        std::size_t edge_start,
        std::size_t vertex
    ){
        const Vector offset = vector_from(
            convex_polygon[edge_start], convex_polygon[vertex]
        );
        return detail::absolute_product_difference(
            edge.x, offset.y, edge.y, offset.x
        );
    };
    const auto same_area = [](
        const detail::DoubleUnsignedWide& first,
        const detail::DoubleUnsignedWide& second
    ){
        return first.high == second.high && first.low == second.low;
    };

    std::size_t opposite = 1;
    const Vector first_edge = vector_from(
        convex_polygon[0], convex_polygon[1]
    );
    detail::DoubleUnsignedWide opposite_area =
        doubled_area(first_edge, 0, opposite);
    for(std::size_t candidate = 2; candidate < size; ++candidate){
        const detail::DoubleUnsignedWide candidate_area =
            doubled_area(first_edge, 0, candidate);
        if(detail::double_unsigned_less_equal(
            opposite_area, candidate_area
        )){
            opposite = candidate;
            opposite_area = candidate_area;
        }
    }

    UnsignedWide best = 0;
    for(std::size_t index = 0; index < size; ++index){
        const std::size_t next = (index + 1) % size;
        const Vector edge = vector_from(
            convex_polygon[index], convex_polygon[next]
        );
        opposite_area = doubled_area(edge, index, opposite);
        while(true){
            const std::size_t next_opposite = (opposite + 1) % size;
            const detail::DoubleUnsignedWide next_area =
                doubled_area(edge, index, next_opposite);
            if(detail::double_unsigned_less_equal(
                next_area, opposite_area
            )){
                break;
            }
            opposite = next_opposite;
            opposite_area = next_area;
        }

        best = std::max(best, squared_distance(
            convex_polygon[index], convex_polygon[opposite]
        ));
        best = std::max(best, squared_distance(
            convex_polygon[next], convex_polygon[opposite]
        ));

        const std::size_t next_opposite = (opposite + 1) % size;
        const detail::DoubleUnsignedWide next_area =
            doubled_area(edge, index, next_opposite);
        if(same_area(opposite_area, next_area)){
            best = std::max(best, squared_distance(
                convex_polygon[index], convex_polygon[next_opposite]
            ));
            best = std::max(best, squared_distance(
                convex_polygon[next], convex_polygon[next_opposite]
            ));
        }
    }
    return best;
}

inline std::optional<UnsignedWide> point_set_diameter_squared(
    std::vector<Point> points
){
    if(points.empty()) return std::nullopt;
    return convex_diameter_squared(convex_hull(std::move(points)));
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_DISTANCE_HPP_INCLUDED
