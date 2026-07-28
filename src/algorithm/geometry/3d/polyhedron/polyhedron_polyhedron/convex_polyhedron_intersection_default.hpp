#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DEFAULT_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "convex_polyhedron_intersection_via_halfspaces.hpp"
#include "../point_set/convex_hull_3d_default.hpp"
#include "../../predicate/polyhedron_point/convex_polyhedron_contains.hpp"
#include "../../index_set/polyhedron/convex_polyhedron_edges.hpp"
#include "../../result/polyhedron_segment/convex_polyhedron_segment_intersection.hpp"

inline ConvexPolyhedron3 convex_polyhedron_intersection(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.affine_dimension == 3 && second.affine_dimension == 3){
        try{
            return convex_polyhedron_intersection_via_halfspaces(
                first, second
            );
        }catch(const std::domain_error&){
            // Degenerate dual configurations use the candidate-point fallback.
        }
    }
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DEFAULT_HPP_INCLUDED
