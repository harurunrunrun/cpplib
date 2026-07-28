#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_POINT_SET_POINT_SET_MINKOWSKI_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_POINT_SET_POINT_SET_MINKOWSKI_SUM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../integer/point_point_point/orientation.hpp"
#include "../../point/point_point/add_points.hpp"
#include "../point_set/convex_hull.hpp"

namespace integer_geometry{
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
    int turn = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        turn = orientation(
            polygon[index],
            polygon[(index + 1) % polygon.size()],
            polygon[(index + 2) % polygon.size()]
        );
        if(turn != 0) break;
    }
    if(turn < 0) std::reverse(polygon.begin(), polygon.end());
    const auto first = std::min_element(
        polygon.begin(), polygon.end(), by_y_then_x
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
            - first.size()){
        throw std::length_error("integer geometry Minkowski sum is too large");
    }
    std::vector<Point> result;
    result.reserve(first.size() + second.size());
    Point current = add_points(first.front(), second.front());
    result.push_back(current);
    std::size_t first_index = 0;
    std::size_t second_index = 0;
    while(first_index < first.size() || second_index < second.size()){
        Vector step;
        if(first_index == first.size()){
            step = second[(second_index + 1) % second.size()]
                - second[second_index];
            ++second_index;
        }else if(second_index == second.size()){
            step = first[(first_index + 1) % first.size()]
                - first[first_index];
            ++first_index;
        }else{
            const Vector first_edge =
                first[(first_index + 1) % first.size()] - first[first_index];
            const Vector second_edge =
                second[(second_index + 1) % second.size()]
                    - second[second_index];
            const int determinant = cross(first_edge, second_edge).sign();
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
        current = current + step;
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
        convex_hull(std::move(first)), convex_hull(std::move(second))
    );
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_POINT_SET_POINT_SET_MINKOWSKI_SUM_HPP_INCLUDED
