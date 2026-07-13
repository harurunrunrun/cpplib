#pragma once

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

#include "convex_calipers_common.hpp"

struct ConvexPolygonMaximumAreaTriangleResult{
    std::array<std::size_t, 3> indices{
        CONVEX_POLYGON_NPOS,
        CONVEX_POLYGON_NPOS,
        CONVEX_POLYGON_NPOS,
    };
    long double doubled_area = 0.0L;

    bool valid() const{ return indices[0] != CONVEX_POLYGON_NPOS; }
    long double area() const{ return doubled_area / 2.0L; }
};

inline ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(const NormalizedConvexPolygon& polygon){
    using namespace rotating_calipers_detail;

    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count < 3) return {};

    ConvexPolygonMaximumAreaTriangleResult result{{0, 1, 2},
        doubled_triangle_area(vertices[0], vertices[1], vertices[2])};
    for(std::size_t first = 0; first + 2 < count; ++first){
        std::size_t third = first + 2;
        for(std::size_t second = first + 1; second + 1 < count; ++second){
            if(third <= second) third = second + 1;
            while(third + 1 < count){
                const long double current = doubled_triangle_area(
                    vertices[first], vertices[second], vertices[third]
                );
                const long double next = doubled_triangle_area(
                    vertices[first], vertices[second], vertices[third + 1]
                );
                if(scalar_compare(next, current) < 0) break;
                ++third;
            }
            const long double candidate = doubled_triangle_area(
                vertices[first], vertices[second], vertices[third]
            );
            const std::array<std::size_t, 3> indices{first, second, third};
            const int comparison = scalar_compare(candidate, result.doubled_area);
            if(comparison > 0 || (comparison == 0 && indices < result.indices)){
                result = {indices, candidate};
            }
        }
    }
    return result;
}

inline ConvexPolygonMaximumAreaTriangleResult
convex_polygon_maximum_area_triangle(std::vector<Point> polygon){
    return convex_polygon_maximum_area_triangle(
        NormalizedConvexPolygon(std::move(polygon))
    );
}
