#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <variant>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "parallel.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"
#include "three_plane_intersection.hpp"

namespace halfspace_intersection_3d_detail{

inline bool feasible_point(
    const std::vector<Plane3>& halfspaces,
    const Point3& point
){
    for(const Plane3& halfspace: halfspaces){
        const Point3 difference = point - halfspace.point;
        const long double scale = std::max({
            1.0L, std::abs(difference.x), std::abs(difference.y),
            std::abs(difference.z)
        });
        if(dot(halfspace.normal, difference) > GEOMETRY3D_EPS * scale){
            return false;
        }
    }
    return true;
}

inline bool feasible_direction(
    const std::vector<Plane3>& halfspaces,
    Point3 direction
){
    const long double length = std::hypot(direction.x, direction.y, direction.z);
    if(length <= GEOMETRY3D_EPS) return false;
    direction /= length;
    for(const Plane3& halfspace: halfspaces){
        if(dot(halfspace.normal, direction) > 8.0L * GEOMETRY3D_EPS){
            return false;
        }
    }
    return true;
}

inline bool has_nonzero_recession_direction(
    const std::vector<Plane3>& halfspaces
){
    const std::array<Point3, 3> axes{
        Point3{1, 0, 0}, Point3{0, 1, 0}, Point3{0, 0, 1}
    };
    for(const Point3& axis: axes){
        if(feasible_direction(halfspaces, axis)
            || feasible_direction(halfspaces, -axis)) return true;
    }
    for(const Plane3& halfspace: halfspaces){
        if(feasible_direction(halfspaces, halfspace.normal)
            || feasible_direction(halfspaces, -halfspace.normal)) return true;
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            const Point3 direction = cross(
                halfspaces[first].normal, halfspaces[second].normal
            );
            if(feasible_direction(halfspaces, direction)
                || feasible_direction(halfspaces, -direction)) return true;
        }
    }
    return false;
}

inline bool has_feasible_point(const std::vector<Plane3>& halfspaces){
    if(feasible_point(halfspaces, {0, 0, 0})) return true;
    for(const Plane3& halfspace: halfspaces){
        const Point3 closest_to_origin =
            halfspace.normal * dot(halfspace.normal, halfspace.point);
        if(feasible_point(halfspaces, closest_to_origin)) return true;
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            if(parallel(
                halfspaces[first].normal, halfspaces[second].normal
            )) continue;
            const Line3 line = plane_plane_intersection(
                halfspaces[first], halfspaces[second]
            );
            if(feasible_point(halfspaces, line.a)) return true;
        }
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            for(std::size_t third = second + 1; third < halfspaces.size(); ++third){
                const ThreePlaneIntersection3 intersection = three_plane_intersection(
                    halfspaces[first], halfspaces[second], halfspaces[third]
                );
                if(const Point3* point = std::get_if<Point3>(&intersection)){
                    if(feasible_point(halfspaces, *point)) return true;
                }else if(const Line3* line = std::get_if<Line3>(&intersection)){
                    if(feasible_point(halfspaces, line->a)) return true;
                }else if(const Plane3* plane = std::get_if<Plane3>(&intersection)){
                    const Point3 point = plane->normal
                        * dot(plane3_unit_normal(*plane), plane->point);
                    if(feasible_point(halfspaces, point)) return true;
                }
            }
        }
    }
    return false;
}

}  // namespace halfspace_intersection_3d_detail

inline ConvexPolyhedron3 halfspace_intersection_3d(
    const std::vector<Plane3>& halfspaces
){
    using namespace halfspace_intersection_3d_detail;
    std::vector<Plane3> normalized;
    normalized.reserve(halfspaces.size());
    for(const Plane3& halfspace: halfspaces){
        if(!geometry3d_is_finite(halfspace.point)
            || !geometry3d_is_finite(halfspace.normal))[[unlikely]]{
            throw std::invalid_argument("non-finite 3D halfspace");
        }
        normalized.push_back({halfspace.point, plane3_unit_normal(halfspace)});
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
                const long double scale = std::max({
                    1.0L, std::abs(point.x), std::abs(point.y), std::abs(point.z),
                    std::abs(other.x), std::abs(other.y), std::abs(other.z)
                });
                return std::hypot(
                    point.x - other.x, point.y - other.y, point.z - other.z
                ) <= 8.0L * GEOMETRY3D_EPS * scale;
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
