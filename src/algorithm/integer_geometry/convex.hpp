#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_CONVEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_CONVEX_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>
#include <utility>

#include "polygon.hpp"

namespace integer_geometry{

inline std::vector<Point> convex_hull(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if(points.size() <= 1) return points;

    if(points.size()
        > std::numeric_limits<std::size_t>::max() / 2)[[unlikely]]{
        throw std::length_error(
            "integer geometry convex hull input is too large"
        );
    }
    std::vector<Point> hull(2 * points.size());
    std::size_t size = 0;
    for(const Point& point: points){
        while(size >= 2
            && orientation(hull[size - 2], hull[size - 1], point) <= 0){
            --size;
        }
        hull[size++] = point;
    }
    const std::size_t lower_size = size;
    for(std::size_t index = points.size() - 1; index-- > 0;){
        const Point& point = points[index];
        while(size > lower_size
            && orientation(hull[size - 2], hull[size - 1], point) <= 0){
            --size;
        }
        hull[size++] = point;
    }
    hull.resize(size - 1);
    return hull;
}

inline bool is_convex(const std::vector<Point>& polygon, bool strict = false){
    if(polygon.size() < 3) return false;
    int direction = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const int turn = orientation(
            polygon[index],
            polygon[(index + 1) % polygon.size()],
            polygon[(index + 2) % polygon.size()]
        );
        if(turn == 0){
            if(strict) return false;
            continue;
        }
        if(direction != 0 && direction != turn) return false;
        direction = turn;
    }
    return direction != 0;
}

namespace detail{

inline Point checked_point(Wide x, Wide y){
    constexpr Wide minimum = std::numeric_limits<Coordinate>::min();
    constexpr Wide maximum = std::numeric_limits<Coordinate>::max();
    if(x < minimum || maximum < x || y < minimum || maximum < y){
        throw std::overflow_error("integer geometry coordinate overflow");
    }
    return {static_cast<Coordinate>(x), static_cast<Coordinate>(y)};
}
}  // namespace detail


inline Point add_points(const Point& first, const Point& second){
    return detail::checked_point(
        static_cast<Wide>(first.x) + static_cast<Wide>(second.x),
        static_cast<Wide>(first.y) + static_cast<Wide>(second.y)
    );
}

namespace detail{

inline void canonicalize_convex_polygon(std::vector<Point>& polygon){
    const auto by_y_then_x = [](const Point& left, const Point& right){
        return left.y < right.y
            || (left.y == right.y && left.x < right.x);
    };
    if(polygon.size() <= 1) return;
    if(polygon.size() == 2){
        if(by_y_then_x(polygon[1], polygon[0])){
            std::swap(polygon[0], polygon[1]);
        }
        return;
    }
    int direction = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        direction = orientation(
            polygon[index],
            polygon[(index + 1) % polygon.size()],
            polygon[(index + 2) % polygon.size()]
        );
        if(direction != 0) break;
    }
    if(direction < 0){
        std::reverse(polygon.begin(), polygon.end());
    }
    const auto first = std::min_element(
        polygon.begin(), polygon.end(),
        by_y_then_x
    );
    std::rotate(polygon.begin(), first, polygon.end());
}

}  // namespace detail

inline std::vector<Point> minkowski_sum_convex(
    std::vector<Point> first,
    std::vector<Point> second
){
    if(first.empty() || second.empty()) return {};
    detail::canonicalize_convex_polygon(first);
    detail::canonicalize_convex_polygon(second);

    if(first.size() == 1){
        std::vector<Point> result;
        result.reserve(second.size());
        for(const Point& point: second){
            result.push_back(add_points(first.front(), point));
        }
        return result;
    }
    if(second.size() == 1){
        std::vector<Point> result;
        result.reserve(first.size());
        for(const Point& point: first){
            result.push_back(add_points(point, second.front()));
        }
        return result;
    }
    if(second.size() > std::numeric_limits<std::size_t>::max()
            - first.size())[[unlikely]]{
        throw std::length_error(
            "integer geometry Minkowski sum input is too large"
        );
    }
    std::vector<Point> result;
    result.reserve(first.size() + second.size());
    Point current = add_points(first.front(), second.front());
    result.push_back(current);
    std::size_t first_index = 0;
    std::size_t second_index = 0;
    while(first_index < first.size() || second_index < second.size()){
        Vector step{};
        if(first_index == first.size()){
            step = vector_from(
                second[second_index],
                second[(second_index + 1) % second.size()]
            );
            ++second_index;
        }else if(second_index == second.size()){
            step = vector_from(
                first[first_index],
                first[(first_index + 1) % first.size()]
            );
            ++first_index;
        }else{
            const Vector first_edge = vector_from(
                first[first_index],
                first[(first_index + 1) % first.size()]
            );
            const Vector second_edge = vector_from(
                second[second_index],
                second[(second_index + 1) % second.size()]
            );
            const int determinant = detail::product_difference_sign(
                first_edge.x, second_edge.y,
                first_edge.y, second_edge.x
            );
            if(determinant > 0){
                step = first_edge;
                ++first_index;
            }else if(determinant < 0){
                step = second_edge;
                ++second_index;
            }else{
                step = first_edge + second_edge;
                ++first_index;
                ++second_index;
            }
        }
        current = detail::checked_point(
            static_cast<Wide>(current.x) + step.x,
            static_cast<Wide>(current.y) + step.y
        );
        if(first_index < first.size() || second_index < second.size()){
            result.push_back(current);
        }
    }
    return result;
}

inline std::vector<Point> minkowski_sum(
    std::vector<Point> first,
    std::vector<Point> second
){
    return minkowski_sum_convex(
        convex_hull(std::move(first)),
        convex_hull(std::move(second))
    );
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_CONVEX_HPP_INCLUDED
