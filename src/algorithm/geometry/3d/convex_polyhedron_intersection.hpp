#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_HPP_INCLUDED

#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron_contains.hpp"
#include "convex_polyhedron_edges.hpp"
#include "convex_polyhedron_segment_intersection.hpp"

inline ConvexPolyhedron3 convex_polyhedron_intersection(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    std::vector<Point3> points;
    for(const Point3& point: first.vertices){
        if(convex_polyhedron_contains(second, point)) points.push_back(point);
    }
    for(const Point3& point: second.vertices){
        if(convex_polyhedron_contains(first, point)) points.push_back(point);
    }
    const auto add_clipped_edges = [&](const ConvexPolyhedron3& source,
                                       const ConvexPolyhedron3& clipper){
        for(const auto& edge: convex_polyhedron_edges(source)){
            const auto clipped = convex_polyhedron_segment_intersection(
                clipper, {source.vertices[edge[0]], source.vertices[edge[1]]}
            );
            if(clipped){
                points.push_back(clipped->a);
                points.push_back(clipped->b);
            }
        }
    };
    add_clipped_edges(first, second);
    add_clipped_edges(second, first);
    return convex_hull_3d(std::move(points));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_HPP_INCLUDED
