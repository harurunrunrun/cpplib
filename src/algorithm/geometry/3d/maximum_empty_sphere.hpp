#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <variant>
#include <vector>

#include "convex_polyhedron3.hpp"
#include "convex_polyhedron_contains.hpp"
#include "convex_polyhedron_edges.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "segment_plane_intersection.hpp"
#include "three_plane_intersection.hpp"
#include "voronoi_diagram_3d.hpp"

namespace maximum_empty_sphere_detail{

inline Plane3 bisector_plane(const Point3& first, const Point3& second){
    return {
        {
            std::midpoint(first.x, second.x),
            std::midpoint(first.y, second.y),
            std::midpoint(first.z, second.z),
        },
        geometry3d_normalized_difference(second, first).value,
    };
}

inline bool inside_bounds(
    const ConvexPolyhedron3& bounds,
    const Point3& point
){
    return convex_polyhedron_contains(bounds, point);
}

inline long double nearest_site_distance(
    const Point3& point,
    const std::vector<Point3>& sites
){
    long double result = std::numeric_limits<long double>::infinity();
    for(const Point3& site: sites){
        const Geometry3DNormalizedDifference difference =
            geometry3d_normalized_difference(point, site);
        const long double normalized_distance = std::hypot(
            difference.value.x,
            difference.value.y,
            difference.value.z
        );
        if(normalized_distance >
            std::numeric_limits<long double>::max() / difference.scale){
            throw std::overflow_error(
                "maximum empty sphere radius is not representable"
            );
        }
        result = std::min(
            result, normalized_distance * difference.scale
        );
    }
    return result;
}

}  // namespace maximum_empty_sphere_detail

inline Sphere3 maximum_empty_sphere(
    const std::vector<Point3>& input_sites,
    const ConvexPolyhedron3& bounds
){
    using namespace maximum_empty_sphere_detail;
    if(bounds.affine_dimension != 3 || bounds.vertices.empty())[[unlikely]]{
        throw std::invalid_argument(
            "maximum_empty_sphere requires nonempty three-dimensional bounds"
        );
    }
    const VoronoiDiagram3 voronoi = voronoi_diagram_3d(input_sites);
    const std::vector<Point3>& sites = voronoi.sites;
    if(sites.empty())[[unlikely]]{
        throw std::invalid_argument("maximum_empty_sphere requires a site");
    }
    for(const Point3& site: sites){
        if(!geometry3d_is_finite(site))[[unlikely]]{
            throw std::invalid_argument("non-finite maximum empty sphere site");
        }
    }

    Sphere3 best{{}, -1.0L};
    const auto consider = [&](const Point3& candidate){
        if(!geometry3d_is_finite(candidate) || !inside_bounds(bounds, candidate)){
            return;
        }
        const long double radius = nearest_site_distance(candidate, sites);
        if(radius > best.radius) best = {candidate, radius};
    };
    for(const Point3& vertex: bounds.vertices) consider(vertex);
    for(const Point3& vertex: voronoi.finite_vertices) consider(vertex);

    const auto edges = convex_polyhedron_edges(bounds);
    for(const auto& edge: edges){
        const Segment3 segment{
            bounds.vertices[edge[0]], bounds.vertices[edge[1]]
        };
        for(std::size_t first = 0; first < sites.size(); ++first){
            for(std::size_t second = first + 1; second < sites.size(); ++second){
                try{
                    const auto intersection = segment_plane_intersection(
                        segment, bisector_plane(sites[first], sites[second])
                    );
                    if(intersection) consider(*intersection);
                }catch(const std::overflow_error&){
                }
            }
        }
    }

    for(const auto& face: bounds.faces){
        const Point3& first_vertex = bounds.vertices[face[0]];
        const Point3 first_direction =
            geometry3d_normalized_difference(
                bounds.vertices[face[1]], first_vertex
            ).value;
        const Point3 second_direction =
            geometry3d_normalized_difference(
                bounds.vertices[face[2]], first_vertex
            ).value;
        const Plane3 face_plane{
            first_vertex,
            cross(first_direction, second_direction),
        };
        for(std::size_t first = 0; first < sites.size(); ++first){
            for(std::size_t second = first + 1; second < sites.size(); ++second){
                for(std::size_t third = second + 1;
                    third < sites.size(); ++third){
                    try{
                        const ThreePlaneIntersection3 intersection =
                            three_plane_intersection(
                                face_plane,
                                bisector_plane(sites[first], sites[second]),
                                bisector_plane(sites[first], sites[third])
                            );
                        if(const Point3* point =
                            std::get_if<Point3>(&intersection)){
                            consider(*point);
                        }
                    }catch(const std::overflow_error&){
                    }
                }
            }
        }
    }
    if(best.radius < 0.0L)[[unlikely]]{
        throw std::domain_error("maximum_empty_sphere bounds have no candidate");
    }
    return best;
}
