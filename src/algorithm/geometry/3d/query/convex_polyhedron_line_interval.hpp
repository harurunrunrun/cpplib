#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_LINE_INTERVAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_LINE_INTERVAL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>

#include "../core/convex_polyhedron3.hpp"
#include "../point/cross.hpp"
#include "../scalar/dot.hpp"
#include "../predicate/is_finite.hpp"

namespace convex_polyhedron_interval_detail{

inline std::optional<std::pair<long double, long double>> clip(
    const ConvexPolyhedron3& polyhedron,
    const Point3& origin,
    const Point3& through,
    long double lower,
    long double upper
){
    if(polyhedron.affine_dimension != 3)[[unlikely]]{
        throw std::invalid_argument("intersection interval requires a 3D polyhedron");
    }
    if(!geometry3d_is_finite(origin) || !geometry3d_is_finite(through))[[unlikely]]{
        throw std::invalid_argument("non-finite interval line");
    }
    long double scale = std::max({
        std::abs(origin.x), std::abs(origin.y), std::abs(origin.z),
        std::abs(through.x), std::abs(through.y), std::abs(through.z)
    });
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate interval line");
    }
    const Point3 scaled_origin = origin / scale;
    const Point3 scaled_direction = through / scale - scaled_origin;
    if(std::hypot(
        scaled_direction.x, scaled_direction.y, scaled_direction.z
    ) == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate interval line");
    }
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        const Point3 first = polyhedron.vertices[face[0]] / scale;
        const Point3 second = polyhedron.vertices[face[1]] / scale;
        const Point3 third = polyhedron.vertices[face[2]] / scale;
        const Point3 normal = cross(second - first, third - first);
        const long double offset = dot(normal, scaled_origin - first);
        const long double slope = dot(normal, scaled_direction);
        const long double tolerance = 64.0L
            * std::numeric_limits<long double>::epsilon()
            * std::max(1.0L, std::hypot(normal.x, normal.y, normal.z));
        if(std::abs(slope) <= tolerance){
            if(offset > tolerance) return std::nullopt;
            continue;
        }
        const long double bound = -offset / slope;
        if(slope > 0.0L) upper = std::min(upper, bound);
        else lower = std::max(lower, bound);
        if(lower > upper + tolerance) return std::nullopt;
    }
    return std::pair{lower, upper};
}

}  // namespace convex_polyhedron_interval_detail

inline std::optional<std::pair<long double, long double>>
convex_polyhedron_line_interval(
    const ConvexPolyhedron3& polyhedron,
    const Line3& line
){
    return convex_polyhedron_interval_detail::clip(
        polyhedron, line.a, line.b,
        -std::numeric_limits<long double>::infinity(),
        std::numeric_limits<long double>::infinity()
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_LINE_INTERVAL_HPP_INCLUDED
