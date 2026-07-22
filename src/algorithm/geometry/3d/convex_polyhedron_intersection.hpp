#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTION_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron_contains.hpp"
#include "convex_polyhedron_edges.hpp"
#include "convex_polyhedron_segment_intersection.hpp"
#include "cross.hpp"
#include "halfspace_intersection_3d.hpp"

namespace convex_polyhedron_intersection_detail{

inline void append_halfspaces(
    const ConvexPolyhedron3& polyhedron,
    std::vector<Plane3>& halfspaces
){
    if(polyhedron.affine_dimension != 3
        || polyhedron.vertices.empty()
        || polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument(
            "halfspace intersection requires a full-dimensional polyhedron"
        );
    }
    halfspaces.reserve(halfspaces.size() + polyhedron.faces.size());
    for(const auto& face: polyhedron.faces){
        for(const std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::invalid_argument(
                    "convex polyhedron face index is out of range"
                );
            }
        }
        const Point3& first = polyhedron.vertices[face[0]];
        const Point3& second = polyhedron.vertices[face[1]];
        const Point3& third = polyhedron.vertices[face[2]];
        halfspaces.push_back({
            first,
            cross(second - first, third - first),
        });
    }
}

inline std::vector<Plane3> combined_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    std::vector<Plane3> halfspaces;
    halfspaces.reserve(first.faces.size() + second.faces.size());
    append_halfspaces(first, halfspaces);
    append_halfspaces(second, halfspaces);
    return halfspaces;
}

}  // namespace convex_polyhedron_intersection_detail

inline ConvexPolyhedron3
convex_polyhedron_intersection_via_halfspaces_with_interior_point(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const Point3& strict_interior_point
){
    return halfspace_intersection_3d_with_interior_point(
        convex_polyhedron_intersection_detail::combined_halfspaces(
            first, second
        ),
        strict_interior_point
    );
}

inline ConvexPolyhedron3 convex_polyhedron_intersection_via_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.vertices.empty() || second.vertices.empty()){
        return {-1, {}, {}};
    }
    return halfspace_intersection_3d(
        convex_polyhedron_intersection_detail::combined_halfspaces(
            first, second
        )
    );
}

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
