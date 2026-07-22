#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_CONVEX_POLYGON_SUPPORT_VERTEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_CONVEX_POLYGON_SUPPORT_VERTEX_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "../detail/convex_calipers_detail.hpp"

inline std::size_t convex_polygon_support_vertex(
    const NormalizedConvexPolygon& polygon,
    const Point& direction
){
    using namespace rotating_calipers_detail;

    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count == 0) return CONVEX_POLYGON_NPOS;
    if(count == 1) return 0;
    if(count == 2){
        const int comparison = scalar_compare(
            dot(vertices[1], direction), dot(vertices[0], direction)
        );
        return comparison > 0 ? 1 : 0;
    }
    if(advanced_geometry_detail::length(direction) <= GEOMETRY_EPS) return 0;

    const std::vector<long double>& edge_angles = polygon.edge_angles();
    const long double two_pi = 2.0L * std::acos(-1.0L);
    long double target = std::atan2(direction.y, direction.x)
        + std::acos(-1.0L) / 2.0L;
    while(target < edge_angles.front()) target += two_pi;
    while(target >= edge_angles.front() + two_pi) target -= two_pi;

    const auto iterator = std::lower_bound(
        edge_angles.begin(), edge_angles.end(), target
    );
    const std::size_t transition = iterator == edge_angles.end()
        ? 0
        : static_cast<std::size_t>(iterator - edge_angles.begin());

    std::size_t result = transition;
    long double best = dot(vertices[result], direction);
    for(int offset = -2; offset <= 2; ++offset){
        const std::size_t candidate = (
            transition + count + static_cast<std::size_t>(offset + 2)
            - 2
        ) % count;
        const long double value = dot(vertices[candidate], direction);
        const int comparison = scalar_compare(value, best);
        if(comparison > 0 || (comparison == 0 && candidate < result)){
            result = candidate;
            best = value;
        }
    }
    return result;
}

inline std::size_t convex_polygon_support_vertex(
    std::vector<Point> polygon,
    const Point& direction
){
    return convex_polygon_support_vertex(
        NormalizedConvexPolygon(std::move(polygon)), direction
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_CONVEX_POLYGON_SUPPORT_VERTEX_HPP_INCLUDED
