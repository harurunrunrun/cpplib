#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_HALFSPACE_INTERSECTION_3D_DEFAULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_HALFSPACE_INTERSECTION_3D_DEFAULT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

#include "halfspace_intersection_3d_detail.hpp"

inline ConvexPolyhedron3 halfspace_intersection_3d(
    const std::vector<Plane3>& halfspaces
){
    using namespace halfspace_intersection_3d_detail;
    std::vector<Plane3> normalized;
    normalized.reserve(halfspaces.size());
    for(const Plane3& halfspace: halfspaces){
        normalized.push_back(halfspace);
    }
    validate_halfspaces(normalized);
    try{
        const auto feasible = minimum_norm_feasible_point(normalized);
        if(feasible && feasible_point(normalized, *feasible)){
            const auto interior = strict_interior_point(
                normalized, *feasible
            );
            if(interior){
                return intersection_from_strict_interior(
                    normalized, *interior
                );
            }
        }
    }catch(const std::overflow_error&){
        // Unrepresentable intermediate projections use the exhaustive path.
    }
    if(!has_feasible_point(normalized)) return {-1, {}, {}};
    if(has_nonzero_recession_direction(normalized))[[unlikely]]{
        throw std::domain_error("3D halfspace intersection is unbounded");
    }

    std::vector<Point3> vertices;
    const auto add_unique = [&](const Point3& point){
        if(!feasible_point(normalized, point)) return;
        const bool duplicate = std::any_of(
            vertices.begin(), vertices.end(), [&](const Point3& other){
                return point == other;
            }
        );
        if(!duplicate) vertices.push_back(point);
    };
    for(std::size_t first = 0; first < normalized.size(); ++first){
        for(std::size_t second = first + 1; second < normalized.size(); ++second){
            for(std::size_t third = second + 1; third < normalized.size(); ++third){
                const ThreePlaneIntersection3 intersection = three_plane_intersection(
                    normalized[first], normalized[second], normalized[third]
                );
                if(const Point3* point = std::get_if<Point3>(&intersection)){
                    add_unique(*point);
                }
            }
        }
    }
    if(vertices.empty())[[unlikely]]{
        throw std::domain_error(
            "bounded 3D halfspace intersection has no enumerable vertex"
        );
    }
    return convex_hull_3d(std::move(vertices));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_HALFSPACE_INTERSECTION_3D_DEFAULT_HPP_INCLUDED
