#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <limits>
#include <optional>
#include <variant>
#include <vector>

#include "convex_hull_3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "parallel.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"
#include "plane_plane_intersection.hpp"
#include "three_plane_intersection.hpp"

namespace halfspace_intersection_3d_detail{

inline bool feasible_point(
    const std::vector<Plane3>& halfspaces,
    const Point3& point
){
    for(const Plane3& halfspace: halfspaces){
        const auto product = geometry3d_plane_numeric_detail::exact_dot_difference(
            halfspace.normal, point, halfspace.point
        );
        if(geometry3d_plane_numeric_detail::exact_dot_sign(product) > 0) return false;
    }
    return true;
}

inline bool feasible_direction(
    const std::vector<Plane3>& halfspaces,
    Point3 direction
){
    const long double scale = std::max({
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    });
    if(scale == 0.0L || !std::isfinite(scale)) return false;
    direction /= scale;
    const auto exact_direction =
        geometry3d_plane_numeric_detail::exact_difference(direction, Point3{});
    for(const Plane3& halfspace: halfspaces){
        if(geometry3d_plane_numeric_detail::exact_dot_sign(
            geometry3d_plane_numeric_detail::exact_dot(
                halfspace.normal, exact_direction
            ),
            8.0L
        ) > 0){
            return false;
        }
    }
    return true;
}

inline Point3 scaled_normal(const Plane3& plane){
    const long double scale = std::max({
        std::abs(plane.normal.x), std::abs(plane.normal.y),
        std::abs(plane.normal.z)
    });
    return plane.normal / scale;
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
        for(const Point3& axis: axes){
            const Point3 tangent = cross(scaled_normal(halfspace), axis);
            if(feasible_direction(halfspaces, tangent)
                || feasible_direction(halfspaces, -tangent)) return true;
        }
    }
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size(); ++second){
            const Point3 direction = cross(
                scaled_normal(halfspaces[first]),
                scaled_normal(halfspaces[second])
            );
            if(feasible_direction(halfspaces, direction)
                || feasible_direction(halfspaces, -direction)) return true;
        }
    }
    return false;
}

inline std::optional<Point3> closest_boundary_point_to_origin(
    const Plane3& halfspace
){
    using namespace geometry3d_adaptive_detail;
    const Point3 normal = plane3_unit_normal(halfspace);
    const ExactDyadic distance =
        geometry3d_plane_numeric_detail::exact_dot_difference(
            normal, halfspace.point, Point3{}
        ).value;
    const ExactDyadic one = exact_dyadic(1.0L);
    const auto coordinate = [&](long double component){
        return geometry3d_plane_numeric_detail::exact_ratio(
            multiply(exact_dyadic(component), distance),
            one,
            "halfspace boundary projection is not representable"
        );
    };
    try{
        return Point3{
            coordinate(normal.x),
            coordinate(normal.y),
            coordinate(normal.z),
        };
    }catch(const std::overflow_error&){
        return std::nullopt;
    }
}

inline bool has_feasible_point(const std::vector<Plane3>& halfspaces){
    if(feasible_point(halfspaces, {0, 0, 0})) return true;
    for(const Plane3& halfspace: halfspaces){
        if(feasible_point(halfspaces, halfspace.point)){
            return true;
        }
        const auto closest = closest_boundary_point_to_origin(halfspace);
        if(closest && feasible_point(halfspaces, *closest)) return true;
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
                    const auto point = closest_boundary_point_to_origin(*plane);
                    if(point && feasible_point(halfspaces, *point)){
                        return true;
                    }
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
        static_cast<void>(plane3_unit_normal(halfspace));
        normalized.push_back(halfspace);
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
