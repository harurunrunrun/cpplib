#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "advanced/detail.hpp"
#include "cross.hpp"
#include "distance.hpp"

inline long double convex_diameter(
    const std::vector<Point>& convex_polygon
){
    std::vector<Point> vertices;
    vertices.reserve(convex_polygon.size());
    for(const Point& point: convex_polygon){
        if(vertices.empty() || !(vertices.back() == point)){
            vertices.push_back(point);
        }
    }
    if(vertices.size() >= 2 && vertices.front() == vertices.back()){
        vertices.pop_back();
    }
    const int count = static_cast<int>(vertices.size());
    if(count <= 1) return 0.0L;
    if(count == 2) return distance(vertices[0], vertices[1]);

    int start = -1;
    for(int index = 0; index < count; ++index){
        const Point& previous = vertices[static_cast<std::size_t>(
            (index + count - 1) % count
        )];
        const Point& current = vertices[static_cast<std::size_t>(index)];
        const Point& next = vertices[static_cast<std::size_t>(
            (index + 1) % count
        )];
        if(advanced_geometry_detail::cross_sign(
            current - previous, next - current
        ) != 0){
            start = index;
            break;
        }
    }
    if(start == -1){
        const auto [minimum, maximum] =
            std::minmax_element(vertices.begin(), vertices.end());
        return distance(*minimum, *maximum);
    }

    std::vector<Point> polygon;
    polygon.reserve(vertices.size());
    for(int offset = 0; offset < count; ++offset){
        const Point& point = vertices[static_cast<std::size_t>(
            (start + offset) % count
        )];
        while(polygon.size() >= 2 &&
            advanced_geometry_detail::cross_sign(
                polygon.back() - polygon[polygon.size() - 2],
                point - polygon.back()
            ) == 0){
            polygon.pop_back();
        }
        polygon.push_back(point);
    }

    const int size = static_cast<int>(polygon.size());
    if(size == 2) return distance(polygon[0], polygon[1]);
    long double result = 0.0L;
    int opposite = 1;
    for(int index = 0; index < size; ++index){
        const int next_index = (index + 1) % size;
        const Point edge = polygon[static_cast<std::size_t>(next_index)] -
            polygon[static_cast<std::size_t>(index)];
        while(true){
            const int next_opposite = (opposite + 1) % size;
            const long double current_area = std::abs(cross(
                edge,
                polygon[static_cast<std::size_t>(opposite)] -
                    polygon[static_cast<std::size_t>(index)]
            ));
            const long double next_area = std::abs(cross(
                edge,
                polygon[static_cast<std::size_t>(next_opposite)] -
                    polygon[static_cast<std::size_t>(index)]
            ));
            if(advanced_geometry_detail::scaled_sign(
                next_area - current_area,
                std::max(current_area, next_area)
            ) <= 0) break;
            opposite = next_opposite;
        }
        result = std::max(result, distance(
            polygon[static_cast<std::size_t>(index)],
            polygon[static_cast<std::size_t>(opposite)]
        ));
        result = std::max(result, distance(
            polygon[static_cast<std::size_t>(next_index)],
            polygon[static_cast<std::size_t>(opposite)]
        ));
    }
    return result;
}
