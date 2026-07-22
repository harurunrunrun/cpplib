#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CONVEX_POLYHEDRON_PLANE_SECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CONVEX_POLYHEDRON_PLANE_SECTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "convex_hull_3d.hpp"
#include "../query/convex_polyhedron_edges.hpp"
#include "../scalar/dot.hpp"
#include "../predicate/is_finite.hpp"
#include "../point/line_plane_intersection.hpp"
#include "../point/plane3_unit_normal.hpp"
#include "../core/polygon3.hpp"

namespace convex_polyhedron_plane_section_detail{

struct NormalizedPlaneData{
    Plane3 plane;
};

inline NormalizedPlaneData normalize(
    const ConvexPolyhedron3& polyhedron,
    const Plane3& plane
){
    if(!geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument("non-finite section plane");
    }
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
    }
    static_cast<void>(plane3_unit_normal(plane));
    return {plane};
}

inline int value(const NormalizedPlaneData& data, const Point3& point){
    return geometry3d_plane_numeric_detail::exact_dot_sign(
        geometry3d_plane_numeric_detail::exact_dot_difference(
            data.plane.normal, point, data.plane.point
        ),
        64.0L
    );
}

inline Point3 interpolate(
    const NormalizedPlaneData& data,
    const Point3& first,
    const Point3& second
){
    return line_plane_intersection({first, second}, data.plane);
}

inline int exact_turn(
    const Point3& normal,
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    using namespace geometry3d_adaptive_detail;
    const auto left =
        geometry3d_plane_numeric_detail::exact_difference(second, first);
    const auto right =
        geometry3d_plane_numeric_detail::exact_difference(third, first);
    const std::array<ExactDyadic, 3> product{
        subtract(multiply(left[1], right[2]),
                 multiply(left[2], right[1])),
        subtract(multiply(left[2], right[0]),
                 multiply(left[0], right[2])),
        subtract(multiply(left[0], right[1]),
                 multiply(left[1], right[0])),
    };
    return sign(
        geometry3d_plane_numeric_detail::exact_dot(normal, product).value
    );
}

inline geometry3d_adaptive_detail::ExactDyadic exact_squared_distance(
    const Point3& first,
    const Point3& second
){
    using namespace geometry3d_adaptive_detail;
    const auto difference =
        geometry3d_plane_numeric_detail::exact_difference(first, second);
    return add(
        add(multiply(difference[0], difference[0]),
            multiply(difference[1], difference[1])),
        multiply(difference[2], difference[2])
    );
}

inline Polygon3 convex_order(Polygon3 points, const Point3& normal){
    using namespace geometry3d_adaptive_detail;
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(),
        [](const Point3& left, const Point3& right){
            return left.x == right.x && left.y == right.y
                && left.z == right.z;
        }
    ), points.end());
    if(points.size() <= 2) return points;

    const Point3 anchor = points.front();
    std::sort(
        points.begin() + 1, points.end(),
        [&](const Point3& left, const Point3& right){
            const int turn = exact_turn(normal, anchor, left, right);
            if(turn != 0) return turn > 0;
            return sign(subtract(
                exact_squared_distance(anchor, left),
                exact_squared_distance(anchor, right)
            )) < 0;
        }
    );
    Polygon3 result;
    for(const Point3& point: points){
        while(result.size() >= 2 && exact_turn(
            normal, result[result.size() - 2], result.back(), point
        ) <= 0){
            result.pop_back();
        }
        result.push_back(point);
    }
    return result;
}

}  // namespace convex_polyhedron_plane_section_detail

inline Polygon3 convex_polyhedron_plane_section(
    const ConvexPolyhedron3& polyhedron,
    const Plane3& plane
){
    using namespace convex_polyhedron_plane_section_detail;
    const NormalizedPlaneData data = normalize(polyhedron, plane);
    std::vector<int> values;
    values.reserve(polyhedron.vertices.size());
    for(const Point3& point: polyhedron.vertices){
        values.push_back(value(data, point));
    }
    std::vector<Point3> points;
    for(const auto& edge: convex_polyhedron_edges(polyhedron)){
        const int first = values[edge[0]];
        const int second = values[edge[1]];
        if(first == 0){
            points.push_back(polyhedron.vertices[edge[0]]);
        }
        if(second == 0){
            points.push_back(polyhedron.vertices[edge[1]]);
        }
        if(first * second < 0){
            points.push_back(interpolate(
                data, polyhedron.vertices[edge[0]],
                polyhedron.vertices[edge[1]]
            ));
        }
    }
    if(points.empty()) return {};
    return convex_order(std::move(points), data.plane.normal);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CONVEX_POLYHEDRON_PLANE_SECTION_HPP_INCLUDED
