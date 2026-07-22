#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_MINIMUM_ENCLOSING_RECTANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_MINIMUM_ENCLOSING_RECTANGLE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "../detail/convex_calipers_detail.hpp"
#include "../point/rotate90.hpp"
#include "../point/unit.hpp"

struct MinimumEnclosingRectangleResult{
    std::array<Point, 4> corners{};
    long double width = 0.0L;
    long double height = 0.0L;
    long double area = 0.0L;
    bool valid = false;
};

namespace minimum_enclosing_rectangle_detail{

inline std::size_t support_index(
    const std::vector<Point>& polygon,
    const Point& direction
){
    std::size_t result = 0;
    for(std::size_t index = 1; index < polygon.size(); ++index){
        if(dot(polygon[index], direction) > dot(polygon[result], direction)){
            result = index;
        }
    }
    return result;
}

inline void advance_support(
    const std::vector<Point>& polygon,
    const Point& direction,
    std::size_t& index
){
    const std::size_t count = polygon.size();
    while(dot(polygon[(index + 1) % count], direction)
        > dot(polygon[index], direction)){
        index = (index + 1) % count;
    }
}

}  // namespace minimum_enclosing_rectangle_detail

inline MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    const NormalizedConvexPolygon& polygon
){
    using namespace minimum_enclosing_rectangle_detail;
    const std::vector<Point>& vertices = polygon.vertices();
    const std::size_t count = vertices.size();
    if(count == 0) return {};
    if(count == 1){
        return {{vertices[0], vertices[0], vertices[0], vertices[0]},
            0.0L, 0.0L, 0.0L, true};
    }
    if(count == 2){
        const long double length = abs(vertices[1] - vertices[0]);
        return {{vertices[0], vertices[1], vertices[1], vertices[0]},
            length, 0.0L, 0.0L, true};
    }

    Point initial_axis = unit(vertices[1] - vertices[0]);
    Point initial_normal = rotate90(initial_axis);
    std::size_t maximum_axis = support_index(vertices, initial_axis);
    std::size_t minimum_axis = support_index(vertices, -initial_axis);
    std::size_t maximum_normal = support_index(vertices, initial_normal);
    std::size_t minimum_normal = support_index(vertices, -initial_normal);

    MinimumEnclosingRectangleResult result;
    result.area = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < count; ++index){
        const Point axis = unit(vertices[(index + 1) % count] - vertices[index]);
        const Point normal = rotate90(axis);
        advance_support(vertices, axis, maximum_axis);
        advance_support(vertices, -axis, minimum_axis);
        advance_support(vertices, normal, maximum_normal);
        advance_support(vertices, -normal, minimum_normal);

        const long double minimum_x = dot(vertices[minimum_axis], axis);
        const long double maximum_x = dot(vertices[maximum_axis], axis);
        const long double minimum_y = dot(vertices[minimum_normal], normal);
        const long double maximum_y = dot(vertices[maximum_normal], normal);
        const long double width = maximum_x - minimum_x;
        const long double height = maximum_y - minimum_y;
        const long double candidate_area = width * height;
        if(candidate_area >= result.area) continue;

        result = {{
            axis * minimum_x + normal * minimum_y,
            axis * maximum_x + normal * minimum_y,
            axis * maximum_x + normal * maximum_y,
            axis * minimum_x + normal * maximum_y,
        }, width, height, candidate_area, true};
    }
    return result;
}

inline MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    std::vector<Point> polygon
){
    return minimum_enclosing_rectangle(
        NormalizedConvexPolygon(std::move(polygon))
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_MINIMUM_ENCLOSING_RECTANGLE_HPP_INCLUDED
