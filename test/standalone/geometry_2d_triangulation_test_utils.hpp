#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace geometry_2d_triangulation_test{

struct IndexedPoint{
    Point point;
    std::size_t original_index;
};

inline bool equal_exact(const Point& first, const Point& second){
    return first.x == second.x && first.y == second.y;
}

inline bool between_exact(
    const Point& first,
    const Point& second,
    const Point& point
){
    return std::min(first.x, second.x) <= point.x
        && point.x <= std::max(first.x, second.x)
        && std::min(first.y, second.y) <= point.y
        && point.y <= std::max(first.y, second.y);
}

inline std::vector<IndexedPoint> normalize_exact(
    const std::vector<Point>& polygon
){
    std::vector<IndexedPoint> vertices;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        if(vertices.empty()
            || !equal_exact(vertices.back().point, polygon[index])){
            vertices.push_back({polygon[index], index});
        }
    }
    if(vertices.size() >= 2 && equal_exact(
        vertices.front().point, vertices.back().point
    )){
        vertices.pop_back();
    }
    bool changed = true;
    while(changed && vertices.size() >= 3){
        changed = false;
        for(std::size_t index = 0; index < vertices.size(); ++index){
            const std::size_t before =
                (index + vertices.size() - 1) % vertices.size();
            const std::size_t after =
                (index + 1) % vertices.size();
            if(cross(
                vertices[index].point - vertices[before].point,
                vertices[after].point - vertices[index].point
            ) == 0.0L && between_exact(
                vertices[before].point,
                vertices[after].point,
                vertices[index].point
            )){
                vertices.erase(
                    vertices.begin()
                    + static_cast<std::ptrdiff_t>(index)
                );
                changed = true;
                break;
            }
        }
    }
    return vertices;
}

inline std::pair<std::size_t, std::size_t> edge_key(
    std::size_t first,
    std::size_t second
){
    if(second < first) std::swap(first, second);
    return {first, second};
}

inline bool validate(
    const std::vector<Point>& polygon,
    const std::vector<EarClippingTriangle>& triangles
){
    const std::vector<IndexedPoint> vertices =
        normalize_exact(polygon);
    if(vertices.size() < 3
        || triangles.size() != vertices.size() - 2){
        return false;
    }
    std::set<std::size_t> active_indices;
    std::set<std::pair<std::size_t, std::size_t>> boundary;
    long double polygon_twice_area = 0.0L;
    for(std::size_t index = 0; index < vertices.size(); ++index){
        const IndexedPoint& first = vertices[index];
        const IndexedPoint& second =
            vertices[(index + 1) % vertices.size()];
        active_indices.insert(first.original_index);
        boundary.insert(edge_key(
            first.original_index, second.original_index
        ));
        polygon_twice_area += cross(first.point, second.point);
    }

    std::map<std::pair<std::size_t, std::size_t>, std::size_t>
        edge_occurrences;
    long double triangle_twice_area = 0.0L;
    for(const EarClippingTriangle& triangle: triangles){
        if(triangle[0] == triangle[1]
            || triangle[1] == triangle[2]
            || triangle[2] == triangle[0]){
            return false;
        }
        for(const std::size_t vertex: triangle){
            if(active_indices.count(vertex) == 0) return false;
        }
        const long double area = cross(
            polygon[triangle[1]] - polygon[triangle[0]],
            polygon[triangle[2]] - polygon[triangle[0]]
        );
        if(geometry_sign(area) <= 0) return false;
        triangle_twice_area += area;
        for(std::size_t edge = 0; edge < 3; ++edge){
            ++edge_occurrences[edge_key(
                triangle[edge], triangle[(edge + 1) % 3]
            )];
        }
    }
    for(const auto& [edge, occurrences]: edge_occurrences){
        const std::size_t expected =
            boundary.count(edge) != 0 ? 1 : 2;
        if(occurrences != expected) return false;
    }
    for(const auto& edge: boundary){
        if(edge_occurrences[edge] != 1) return false;
    }

    const long double expected_area = std::abs(polygon_twice_area);
    return std::abs(triangle_twice_area - expected_area)
        <= 1e-8L * std::max(1.0L, expected_area);
}

}  // namespace geometry_2d_triangulation_test
