#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "abs.hpp"
#include "convex_calipers_common.hpp"

inline long double convex_polygon_minimum_width(
    const NormalizedConvexPolygon& polygon
){
    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count < 3) return 0.0L;

    long double result = std::numeric_limits<long double>::infinity();
    std::size_t opposite = 1;
    for(std::size_t index = 0; index < count; ++index){
        const std::size_t next = (index + 1) % count;
        const Point edge = vertices[next] - vertices[index];
        while(true){
            const std::size_t candidate = (opposite + 1) % count;
            const long double current_area = std::abs(cross(
                edge, vertices[opposite] - vertices[index]
            ));
            const long double next_area = std::abs(cross(
                edge, vertices[candidate] - vertices[index]
            ));
            if(geometry_sign(next_area - current_area) <= 0) break;
            opposite = candidate;
        }
        result = std::min(result, std::abs(cross(
            edge, vertices[opposite] - vertices[index]
        )) / abs(edge));
    }
    return result;
}

inline long double convex_polygon_minimum_width(std::vector<Point> polygon){
    return convex_polygon_minimum_width(
        NormalizedConvexPolygon(std::move(polygon))
    );
}
