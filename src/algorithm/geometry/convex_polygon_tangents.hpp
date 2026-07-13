#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "convex_calipers_common.hpp"

struct ConvexPolygonTangentResult{
    std::size_t left = CONVEX_POLYGON_NPOS;
    std::size_t right = CONVEX_POLYGON_NPOS;
};

namespace rotating_calipers_detail{

inline std::size_t right_tangent_candidate(
    const std::vector<Point>& polygon,
    const Point& point
){
    const std::size_t count = polygon.size();
    const auto above = [&](std::size_t first, std::size_t second){
        return oriented_side(
            point,
            cyclic_vertex(polygon, first),
            cyclic_vertex(polygon, second)
        ) > 0;
    };
    const auto below = [&](std::size_t first, std::size_t second){
        return oriented_side(
            point,
            cyclic_vertex(polygon, first),
            cyclic_vertex(polygon, second)
        ) < 0;
    };

    if(below(1, 0) && !above(count - 1, 0)) return 0;
    std::size_t low = 0;
    std::size_t high = count;
    while(high - low > 1){
        const std::size_t middle = low + (high - low) / 2;
        const bool middle_down = below(middle + 1, middle);
        if(middle_down && !above(middle - 1, middle)) return middle;
        const bool low_up = above(low + 1, low);
        if(low_up){
            if(middle_down || above(low, middle)) high = middle;
            else low = middle;
        }else{
            if(!middle_down || !below(low, middle)) low = middle;
            else high = middle;
        }
    }
    return above(low, high) ? low : high % count;
}

inline std::size_t left_tangent_candidate(
    const std::vector<Point>& polygon,
    const Point& point
){
    const std::size_t count = polygon.size();
    const auto above = [&](std::size_t first, std::size_t second){
        return oriented_side(
            point,
            cyclic_vertex(polygon, first),
            cyclic_vertex(polygon, second)
        ) > 0;
    };
    const auto below = [&](std::size_t first, std::size_t second){
        return oriented_side(
            point,
            cyclic_vertex(polygon, first),
            cyclic_vertex(polygon, second)
        ) < 0;
    };

    if(above(count - 1, 0) && !below(1, 0)) return 0;
    std::size_t low = 0;
    std::size_t high = count;
    while(high - low > 1){
        const std::size_t middle = low + (high - low) / 2;
        const bool middle_down = below(middle + 1, middle);
        if(above(middle - 1, middle) && !middle_down) return middle;
        const bool low_down = below(low + 1, low);
        if(low_down){
            if(!middle_down || below(low, middle)) high = middle;
            else low = middle;
        }else{
            if(middle_down || !above(low, middle)) low = middle;
            else high = middle;
        }
    }
    return below(low, high) ? low : high % count;
}

inline bool is_left_tangent(
    const std::vector<Point>& polygon,
    const Point& point,
    std::size_t index
){
    const std::size_t count = polygon.size();
    return oriented_side(point, polygon[index], polygon[(index + count - 1) % count]) >= 0
        && oriented_side(point, polygon[index], polygon[(index + 1) % count]) >= 0;
}

inline bool is_right_tangent(
    const std::vector<Point>& polygon,
    const Point& point,
    std::size_t index
){
    const std::size_t count = polygon.size();
    return oriented_side(point, polygon[index], polygon[(index + count - 1) % count]) <= 0
        && oriented_side(point, polygon[index], polygon[(index + 1) % count]) <= 0;
}

template<class Predicate>
inline std::size_t canonical_tangent(
    const std::vector<Point>& polygon,
    std::size_t candidate,
    Predicate predicate
){
    const std::size_t count = polygon.size();
    std::size_t result = CONVEX_POLYGON_NPOS;
    for(int offset = -1; offset <= 1; ++offset){
        const std::size_t index = (
            candidate + count + static_cast<std::size_t>(offset + 1) - 1
        ) % count;
        if(predicate(index)) result = std::min(result, index);
    }
    if(result == CONVEX_POLYGON_NPOS){
        throw std::runtime_error("failed to locate a convex polygon tangent");
    }
    return result;
}

}  // namespace rotating_calipers_detail

inline ConvexPolygonTangentResult convex_polygon_tangents(
    const NormalizedConvexPolygon& polygon,
    const Point& point
){
    using namespace rotating_calipers_detail;

    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count == 0) return {};
    if(contains_normalized_convex_polygon(vertices, point) != 0){
        throw std::invalid_argument("the tangent source must be outside the convex polygon");
    }
    if(count == 1) return {0, 0};
    if(count == 2){
        const int side = oriented_side(point, vertices[0], vertices[1]);
        if(side > 0) return {0, 1};
        if(side < 0) return {1, 0};
        return {0, 0};
    }

    const std::size_t left_candidate = right_tangent_candidate(vertices, point);
    const std::size_t right_candidate = left_tangent_candidate(vertices, point);
    const std::size_t left = canonical_tangent(
        vertices,
        left_candidate,
        [&](std::size_t index){ return is_left_tangent(vertices, point, index); }
    );
    const std::size_t right = canonical_tangent(
        vertices,
        right_candidate,
        [&](std::size_t index){ return is_right_tangent(vertices, point, index); }
    );
    return {left, right};
}

inline ConvexPolygonTangentResult convex_polygon_tangents(
    std::vector<Point> polygon,
    const Point& point
){
    return convex_polygon_tangents(
        NormalizedConvexPolygon(std::move(polygon)), point
    );
}
