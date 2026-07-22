#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_NORMALIZE_CONVEX_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_NORMALIZE_CONVEX_POLYGON_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "detail.hpp"

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
            start = (index + 1) % count;
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
    std::size_t direction_wraps = 0;
    for(std::size_t index = 0; index < result.size(); ++index){
        const Point current_direction =
            result[(index + 1) % result.size()] - result[index];
        const Point next_direction =
            result[(index + 2) % result.size()]
            - result[(index + 1) % result.size()];
        if(direction_less(next_direction, current_direction)) ++direction_wraps;
    }
    if(direction_wraps != 1){
        throw std::invalid_argument("the polygon boundary winds more than once");
    }
    canonicalize_start(result);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_NORMALIZE_CONVEX_POLYGON_HPP_INCLUDED
