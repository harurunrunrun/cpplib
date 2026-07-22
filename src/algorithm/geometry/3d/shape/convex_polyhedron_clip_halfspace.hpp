#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CONVEX_POLYHEDRON_CLIP_HALFSPACE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CONVEX_POLYHEDRON_CLIP_HALFSPACE_HPP_INCLUDED

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
    std::vector<int> values;
    values.reserve(polyhedron.vertices.size());
    std::vector<Point3> points;
    for(const Point3& point: polyhedron.vertices){
        const int signed_value = value(data, point);
        values.push_back(signed_value);
        const bool inside = keep_negative_side
            ? signed_value <= 0
            : signed_value >= 0;
        if(inside) points.push_back(point);
    }
    for(const auto& edge: convex_polyhedron_edges(polyhedron)){
        const int first = values[edge[0]];
        const int second = values[edge[1]];
        if(first * second < 0){
            points.push_back(interpolate(
                data, polyhedron.vertices[edge[0]],
                polyhedron.vertices[edge[1]]
            ));
        }
    }
    return convex_hull_3d(std::move(points));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CONVEX_POLYHEDRON_CLIP_HALFSPACE_HPP_INCLUDED
