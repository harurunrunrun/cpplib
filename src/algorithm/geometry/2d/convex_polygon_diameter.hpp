#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "convex_polygon_farthest_vertices.hpp"

struct ConvexPolygonDiameterResult{
    std::size_t first = CONVEX_POLYGON_NPOS;
    std::size_t second = CONVEX_POLYGON_NPOS;
    long double squared_distance = 0.0L;

    long double distance() const{ return std::sqrt(squared_distance); }
};

inline ConvexPolygonDiameterResult convex_polygon_diameter(
    const NormalizedConvexPolygon& polygon
){
    using namespace rotating_calipers_detail;

    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count == 0) return {};
    if(count == 1) return {0, 0, 0.0L};

    const std::vector<std::size_t> farthest =
        convex_polygon_farthest_vertices(polygon);
    ConvexPolygonDiameterResult result{0, farthest[0], squared_distance(
        vertices[0], vertices[farthest[0]]
    )};
    if(result.second < result.first) std::swap(result.first, result.second);

    for(std::size_t index = 1; index < count; ++index){
        const std::size_t other = farthest[index];
        const long double candidate_distance = squared_distance(
            vertices[index], vertices[other]
        );
        std::size_t first = std::min(index, other);
        std::size_t second = std::max(index, other);
        const int comparison = scalar_compare(
            candidate_distance, result.squared_distance
        );
        if(comparison > 0 || (comparison == 0
            && std::pair{first, second} < std::pair{result.first, result.second})){
            result = {first, second, candidate_distance};
        }
    }
    return result;
}

inline ConvexPolygonDiameterResult convex_polygon_diameter(
    std::vector<Point> polygon
){
    return convex_polygon_diameter(NormalizedConvexPolygon(std::move(polygon)));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CONVEX_POLYGON_DIAMETER_HPP_INCLUDED
