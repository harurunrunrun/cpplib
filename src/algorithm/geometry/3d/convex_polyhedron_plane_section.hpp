#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron_edges.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "polygon3.hpp"

namespace convex_polyhedron_plane_section_detail{

struct NormalizedPlaneData{
    Point3 point;
    Point3 normal;
    long double coordinate_scale = 1.0L;
    long double tolerance = 0.0L;
};

inline NormalizedPlaneData normalize(
    const ConvexPolyhedron3& polyhedron,
    const Plane3& plane
){
    if(!geometry3d_is_finite(plane.point)
        || !geometry3d_is_finite(plane.normal))[[unlikely]]{
        throw std::invalid_argument("non-finite section plane");
    }
    const long double normal_scale = std::max({
        std::abs(plane.normal.x), std::abs(plane.normal.y),
        std::abs(plane.normal.z)
    });
    if(normal_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero section plane normal");
    }
    long double coordinate_scale = std::max({
        std::abs(plane.point.x), std::abs(plane.point.y),
        std::abs(plane.point.z)
    });
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
        coordinate_scale = std::max({
            coordinate_scale, std::abs(point.x), std::abs(point.y),
            std::abs(point.z)
        });
    }
    if(coordinate_scale == 0.0L) coordinate_scale = 1.0L;
    const Point3 normal = plane.normal / normal_scale;
    return {
        plane.point / coordinate_scale,
        normal,
        coordinate_scale,
        64.0L * std::numeric_limits<long double>::epsilon(),
    };
}

inline long double value(const NormalizedPlaneData& data, const Point3& point){
    return dot(data.normal, point / data.coordinate_scale - data.point);
}

inline Point3 interpolate(
    const Point3& first,
    const Point3& second,
    long double first_value,
    long double second_value
){
    const long double parameter = first_value / (first_value - second_value);
    return first * (1.0L - parameter) + second * parameter;
}

}  // namespace convex_polyhedron_plane_section_detail

inline Polygon3 convex_polyhedron_plane_section(
    const ConvexPolyhedron3& polyhedron,
    const Plane3& plane
){
    using namespace convex_polyhedron_plane_section_detail;
    const NormalizedPlaneData data = normalize(polyhedron, plane);
    std::vector<long double> values;
    values.reserve(polyhedron.vertices.size());
    for(const Point3& point: polyhedron.vertices){
        values.push_back(value(data, point));
    }
    std::vector<Point3> points;
    for(const auto& edge: convex_polyhedron_edges(polyhedron)){
        const long double first = values[edge[0]];
        const long double second = values[edge[1]];
        if(std::abs(first) <= data.tolerance){
            points.push_back(polyhedron.vertices[edge[0]]);
        }
        if(std::abs(second) <= data.tolerance){
            points.push_back(polyhedron.vertices[edge[1]]);
        }
        if((first < -data.tolerance && second > data.tolerance)
            || (first > data.tolerance && second < -data.tolerance)){
            points.push_back(interpolate(
                polyhedron.vertices[edge[0]], polyhedron.vertices[edge[1]],
                first, second
            ));
        }
    }
    if(points.empty()) return {};
    const ConvexPolyhedron3 hull = convex_hull_3d(std::move(points));
    return hull.vertices;
}
