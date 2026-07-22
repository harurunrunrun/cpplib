#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_PLANE_ARRANGEMENT_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_PLANE_ARRANGEMENT_3D_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <variant>
#include <vector>

#include "../point_collection/deduplicate_points_3d.hpp"
#include "../predicate/is_finite.hpp"
#include "../query/line_line_intersection.hpp"
#include "../predicate/linear_coincident.hpp"
#include "../predicate/on_plane.hpp"
#include "../predicate/parallel.hpp"
#include "../point/plane3_unit_normal.hpp"
#include "plane_arrangement_result3.hpp"
#include "plane_plane_intersection.hpp"
#include "../point/three_plane_intersection.hpp"

namespace plane_arrangement_3d_detail{

inline bool same_plane(const Plane3& first, const Plane3& second){
    return parallel(first.normal, second.normal) && on_plane(first, second.point);
}

inline void add_unique_line(std::vector<Line3>& lines, const Line3& line){
    if(std::none_of(lines.begin(), lines.end(), [&](const Line3& other){
        return coincident(line, other);
    })) lines.push_back(line);
}

inline std::uint64_t checked_add(std::uint64_t left, std::uint64_t right){
    if(right > std::numeric_limits<std::uint64_t>::max() - left)[[unlikely]]{
        throw std::overflow_error("3D plane arrangement region count overflow");
    }
    return left + right;
}

inline std::uint64_t line_arrangement_region_count(
    const std::vector<Line3>& lines
){
    std::uint64_t regions = 1;
    for(std::size_t line_index = 0; line_index < lines.size(); ++line_index){
        std::vector<Point3> intersections;
        for(std::size_t previous = 0; previous < line_index; ++previous){
            const LinearIntersection3 intersection = line_line_intersection(
                lines[line_index], lines[previous]
            );
            if(const Point3* point = std::get_if<Point3>(&intersection)){
                intersections.push_back(*point);
            }
        }
        intersections = deduplicate_points_3d(
            intersections, 0.0L, GEOMETRY3D_EPS
        );
        regions = checked_add(
            regions, static_cast<std::uint64_t>(intersections.size()) + 1
        );
    }
    return regions;
}

}  // namespace plane_arrangement_3d_detail

inline PlaneArrangement3 plane_arrangement_3d(
    const std::vector<Plane3>& input
){
    using namespace plane_arrangement_3d_detail;
    PlaneArrangement3 result;
    for(const Plane3& plane: input){
        if(!geometry3d_is_finite(plane.point)
            || !geometry3d_is_finite(plane.normal))[[unlikely]]{
            throw std::invalid_argument("non-finite plane in 3D arrangement");
        }
        const Plane3 normalized{plane.point, plane3_unit_normal(plane)};
        if(std::any_of(
            result.planes.begin(), result.planes.end(),
            [&](const Plane3& other){ return same_plane(normalized, other); }
        )) continue;

        std::vector<Line3> induced_lines;
        for(const Plane3& previous: result.planes){
            if(parallel(normalized.normal, previous.normal)) continue;
            add_unique_line(
                induced_lines, plane_plane_intersection(normalized, previous)
            );
        }
        result.region_count = checked_add(
            result.region_count, line_arrangement_region_count(induced_lines)
        );
        result.planes.push_back(normalized);
    }

    for(std::size_t first = 0; first < result.planes.size(); ++first){
        for(std::size_t second = first + 1; second < result.planes.size(); ++second){
            if(parallel(
                result.planes[first].normal, result.planes[second].normal
            )) continue;
            add_unique_line(result.intersection_lines, plane_plane_intersection(
                result.planes[first], result.planes[second]
            ));
        }
    }

    std::vector<Point3> point_candidates;
    for(std::size_t first = 0; first < result.planes.size(); ++first){
        for(std::size_t second = first + 1;
            second < result.planes.size(); ++second){
            for(std::size_t third = second + 1;
                third < result.planes.size(); ++third){
                const ThreePlaneIntersection3 intersection = three_plane_intersection(
                    result.planes[first], result.planes[second], result.planes[third]
                );
                if(const Point3* point = std::get_if<Point3>(&intersection)){
                    point_candidates.push_back(*point);
                }
            }
        }
    }
    result.intersection_points = deduplicate_points_3d(
        point_candidates, 0.0L, GEOMETRY3D_EPS
    );
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_PLANE_ARRANGEMENT_3D_HPP_INCLUDED
