#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_NORMALIZED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_NORMALIZED_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "convex_calipers_detail.hpp"
#include "convex_polygon_maximum_area_triangle_result.hpp"

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
            const int comparison = scalar_compare(
                candidate, result.doubled_area
            );
            if(comparison > 0 || (comparison == 0 && indices < result.indices)){
                result = {indices, candidate};
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_MAXIMUM_AREA_TRIANGLE_NORMALIZED_HPP_INCLUDED
