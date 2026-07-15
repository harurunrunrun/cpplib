#pragma once

#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron_plane_section.hpp"

inline ConvexPolyhedron3 convex_polyhedron_clip_halfspace(
    const ConvexPolyhedron3& polyhedron,
    const Plane3& boundary,
    bool keep_negative_side = true
){
    using namespace convex_polyhedron_plane_section_detail;
    const NormalizedPlaneData data = normalize(polyhedron, boundary);
    std::vector<long double> values;
    values.reserve(polyhedron.vertices.size());
    std::vector<Point3> points;
    for(const Point3& point: polyhedron.vertices){
        const long double signed_value = value(data, point);
        values.push_back(signed_value);
        const bool inside = keep_negative_side
            ? signed_value <= data.tolerance
            : signed_value >= -data.tolerance;
        if(inside) points.push_back(point);
    }
    for(const auto& edge: convex_polyhedron_edges(polyhedron)){
        const long double first = values[edge[0]];
        const long double second = values[edge[1]];
        if((first < -data.tolerance && second > data.tolerance)
            || (first > data.tolerance && second < -data.tolerance)){
            points.push_back(interpolate(
                polyhedron.vertices[edge[0]], polyhedron.vertices[edge[1]],
                first, second
            ));
        }
    }
    return convex_hull_3d(std::move(points));
}
