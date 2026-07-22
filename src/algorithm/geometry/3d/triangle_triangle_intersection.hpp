#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_TRIANGLE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_TRIANGLE_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <variant>
#include <vector>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_zero_vector.hpp"
#include "norm.hpp"
#include "on_plane.hpp"
#include "on_segment.hpp"
#include "parallel.hpp"
#include "planar_intersection3.hpp"
#include "segment_segment_intersection.hpp"
#include "triangle_area_vector.hpp"
#include "triangle_plane_intersection.hpp"

namespace geometry3d_triangle_intersection_detail{

inline PlanarIntersection3 reduce_polygon(Polygon3 points){
    Polygon3 unique;
    for(const Point3& point: points){
        bool duplicate = false;
        for(const Point3& existing: unique){
            if(existing == point){ duplicate = true; break; }
        }
        if(!duplicate) unique.push_back(point);
    }
    if(unique.empty()) return std::monostate{};
    if(unique.size() == 1) return unique.front();
    bool collinear = true;
    for(std::size_t index = 2; index < unique.size(); ++index){
        if(!is_zero_vector(cross(
            unique[1] - unique[0], unique[index] - unique[0]
        ))){
            collinear = false;
            break;
        }
    }
    if(collinear){
        std::pair<std::size_t, std::size_t> farthest{0, 1};
        long double farthest_distance = norm(unique[1] - unique[0]);
        for(std::size_t first = 0; first < unique.size(); ++first){
            for(std::size_t second = first + 1; second < unique.size(); ++second){
                const long double candidate = norm(unique[second] - unique[first]);
                if(candidate > farthest_distance){
                    farthest_distance = candidate;
                    farthest = {first, second};
                }
            }
        }
        return Segment3{unique[farthest.first], unique[farthest.second]};
    }
    return unique;
}

inline PlanarIntersection3 coplanar_intersection(
    const Triangle3& first,
    Triangle3 second,
    const Point3& normal
){
    if(geometry3d_sign(dot(
        cross(second.b - second.a, second.c - second.a), normal
    )) < 0){
        std::swap(second.b, second.c);
    }
    Polygon3 polygon{first.a, first.b, first.c};
    const std::array<Point3, 3> clip{second.a, second.b, second.c};
    for(std::size_t edge = 0; edge < 3 && !polygon.empty(); ++edge){
        const Point3 edge_start = clip[edge];
        const Point3 edge_end = clip[(edge + 1) % 3];
        Polygon3 next_polygon;
        for(std::size_t index = 0; index < polygon.size(); ++index){
            const Point3 current = polygon[index];
            const Point3 next = polygon[(index + 1) % polygon.size()];
            const long double current_side = dot(
                cross(edge_end - edge_start, current - edge_start), normal
            );
            const long double next_side = dot(
                cross(edge_end - edge_start, next - edge_start), normal
            );
            const bool current_inside = geometry3d_sign(current_side) >= 0;
            const bool next_inside = geometry3d_sign(next_side) >= 0;
            if(current_inside) next_polygon.push_back(current);
            if(current_inside != next_inside){
                const long double parameter = current_side /
                    (current_side - next_side);
                next_polygon.push_back(current + (next - current) * parameter);
            }
        }
        polygon = std::move(next_polygon);
    }
    return reduce_polygon(std::move(polygon));
}

inline PlanarIntersection3 intersect_sections(
    const PlanarIntersection3& first,
    const PlanarIntersection3& second
){
    const auto* first_point = std::get_if<Point3>(&first);
    const auto* second_point = std::get_if<Point3>(&second);
    const auto* first_segment = std::get_if<Segment3>(&first);
    const auto* second_segment = std::get_if<Segment3>(&second);
    if(first_point && second_point){
        return *first_point == *second_point
            ? PlanarIntersection3{*first_point}
            : PlanarIntersection3{std::monostate{}};
    }
    if(first_point && second_segment){
        return on_segment(*second_segment, *first_point)
            ? PlanarIntersection3{*first_point}
            : PlanarIntersection3{std::monostate{}};
    }
    if(first_segment && second_point){
        return on_segment(*first_segment, *second_point)
            ? PlanarIntersection3{*second_point}
            : PlanarIntersection3{std::monostate{}};
    }
    if(first_segment && second_segment){
        const LinearIntersection3 intersection =
            segment_segment_intersection(*first_segment, *second_segment);
        if(const auto* point = std::get_if<Point3>(&intersection)) return *point;
        if(const auto* segment = std::get_if<Segment3>(&intersection)) return *segment;
    }
    return std::monostate{};
}

}  // namespace geometry3d_triangle_intersection_detail

inline PlanarIntersection3 triangle_triangle_intersection(
    const Triangle3& first,
    const Triangle3& second
){
    const Point3 first_normal = triangle_area_vector(first);
    const Point3 second_normal = triangle_area_vector(second);
    if(is_zero_vector(first_normal) || is_zero_vector(second_normal))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const Plane3 first_plane{first.a, first_normal};
    const Plane3 second_plane{second.a, second_normal};
    if(parallel(first_normal, second_normal)){
        if(!on_plane(first_plane, second.a)) return std::monostate{};
        return geometry3d_triangle_intersection_detail::coplanar_intersection(
            first, second, first_normal
        );
    }
    return geometry3d_triangle_intersection_detail::intersect_sections(
        triangle_plane_intersection(first, second_plane),
        triangle_plane_intersection(second, first_plane)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_TRIANGLE_INTERSECTION_HPP_INCLUDED
