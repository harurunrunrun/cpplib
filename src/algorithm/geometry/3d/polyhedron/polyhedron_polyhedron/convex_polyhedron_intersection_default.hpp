#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DEFAULT_HPP_INCLUDED

#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "convex_polyhedron_intersection_via_halfspaces.hpp"
#include "../point_set/convex_hull_3d_default.hpp"
#include "../../aggregate/all/convex_polyhedron_query_hierarchy_3d.hpp"
#include "../../predicate/polyhedron_point/convex_polyhedron_contains.hpp"
#include "../../index_set/polyhedron/convex_polyhedron_edges.hpp"
#include "../../result/polyhedron_segment/convex_polyhedron_segment_intersection.hpp"

inline ConvexPolyhedron3 convex_polyhedron_intersection(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.affine_dimension == 3 && second.affine_dimension == 3){
        const std::vector<Plane3> halfspaces =
            convex_polyhedron_intersection_detail::combined_halfspaces(
                first, second
            );
        if(!convex_polyhedron_intersects_exact(first, second)){
            return {-1, {}, {}};
        }
        try{
            const auto feasible =
                halfspace_intersection_3d_detail::minimum_norm_feasible_point(
                    halfspaces
                );
            if(feasible){
                const auto interior =
                    halfspace_intersection_3d_detail::strict_interior_point(
                        halfspaces, *feasible
                    );
                if(interior){
                    return halfspace_intersection_3d_detail::
                        intersection_from_strict_interior(
                            halfspaces, *interior
                        );
                }
            }
        }catch(const std::overflow_error&){
            // An unrepresentable interior construction uses the mesh fallback.
        }catch(const std::domain_error&){
            // A degenerate dual configuration uses the mesh fallback.
        }
    }

    std::optional<ConvexPolyhedronQueryHierarchy3D> first_hierarchy;
    std::optional<ConvexPolyhedronQueryHierarchy3D> second_hierarchy;
    if(first.affine_dimension == 3) first_hierarchy.emplace(first);
    if(second.affine_dimension == 3) second_hierarchy.emplace(second);
    const auto contains = [](
        const ConvexPolyhedron3& polyhedron,
        const std::optional<ConvexPolyhedronQueryHierarchy3D>& hierarchy,
        const Point3& point
    ){
        return hierarchy ? hierarchy->contains(point)
            : convex_polyhedron_contains(polyhedron, point);
    };

    std::vector<Point3> points;
    for(const Point3& point: first.vertices){
        if(contains(second, second_hierarchy, point)) points.push_back(point);
    }
    for(const Point3& point: second.vertices){
        if(contains(first, first_hierarchy, point)) points.push_back(point);
    }
    const auto add_clipped_edges = [&](const ConvexPolyhedron3& source,
                                       const ConvexPolyhedron3& clipper,
                                       const std::optional<
                                           ConvexPolyhedronQueryHierarchy3D
                                       >& hierarchy){
        for(const auto& edge: convex_polyhedron_edges(source)){
            const Segment3 segment{
                source.vertices[edge[0]], source.vertices[edge[1]]
            };
            const auto clipped = hierarchy
                ? hierarchy->segment_intersection(segment)
                : convex_polyhedron_segment_intersection(clipper, segment);
            if(clipped){
                points.push_back(clipped->a);
                points.push_back(clipped->b);
            }
        }
    };
    add_clipped_edges(first, second, second_hierarchy);
    add_clipped_edges(second, first, first_hierarchy);
    return convex_hull_3d(std::move(points));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYHEDRON_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTION_DEFAULT_HPP_INCLUDED
