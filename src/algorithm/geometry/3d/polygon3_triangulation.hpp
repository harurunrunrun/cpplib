#pragma once

#include <array>
#include <vector>

#include "../2d/simple_polygon_triangulation.hpp"
#include "polygon3_to_2d.hpp"

inline std::vector<std::array<std::size_t, 3>> polygon3_triangulation_indices(
    const Polygon3& polygon
){
    return simple_polygon_triangulation(polygon3_to_2d(polygon).second);
}

inline std::vector<Triangle3> polygon3_triangulation(const Polygon3& polygon){
    const auto indices = polygon3_triangulation_indices(polygon);
    std::vector<Triangle3> result;
    result.reserve(indices.size());
    for(const auto& triangle: indices){
        result.push_back({
            polygon[triangle[0]], polygon[triangle[1]], polygon[triangle[2]]
        });
    }
    return result;
}
