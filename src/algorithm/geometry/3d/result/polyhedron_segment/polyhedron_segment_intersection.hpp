#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_SEGMENT_POLYHEDRON_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_SEGMENT_POLYHEDRON_SEGMENT_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <utility>
#include <variant>
#include <vector>

#include "../../detail/scalar/linear_closest_detail.hpp"
#include "../../predicate/polyhedron_point/polyhedron_contains.hpp"
#include "../../predicate/segment/segment3_is_finite.hpp"
#include "../../result/point_point/point3_normalized_difference_points.hpp"
#include "../../result/segment_triangle/segment_triangle_intersection.hpp"
#include "../../type/definition/polyhedron3.hpp"

inline std::vector<Segment3> polyhedron_segment_intersection(
    const Polyhedron3& polyhedron,
    const Segment3& segment
){
    geometry3d_validate(polyhedron);
    if(!geometry3d_is_finite(segment))[[unlikely]]{
        throw std::invalid_argument("non-finite polyhedron query segment");
    }
    if(segment.a == segment.b){
        return polyhedron_contains_detail::unchecked(polyhedron, segment.a)
            ? std::vector<Segment3>{segment} : std::vector<Segment3>{};
    }

    const Geometry3DNormalizedDifference direction_difference =
        geometry3d_normalized_difference(segment.b, segment.a);
    const Point3 direction = direction_difference.value;
    int axis = std::abs(direction.y) > std::abs(direction.x) ? 1 : 0;
    if((axis == 0 ? std::abs(direction.x) : std::abs(direction.y))
        < std::abs(direction.z)) axis = 2;
    const auto coordinate = [axis](const Point3& point){
        if(axis == 0) return point.x;
        if(axis == 1) return point.y;
        return point.z;
    };
    const long double denominator = coordinate(direction);
    const auto parameter = [&](const Point3& point){
        const Point3 local = geometry3d_safe_normalized_difference(
            point, segment.a, direction_difference.scale
        );
        return std::clamp(coordinate(local) / denominator, 0.0L, 1.0L);
    };
    const auto point_at = [&](long double value){
        return geometry3d_linear_closest_detail::restore(
            direction * value, segment.a, direction_difference.scale
        );
    };

    std::vector<long double> events{0.0L, 1.0L};
    std::vector<long double> boundary_events;
    for(const auto& face: polyhedron.faces){
        const LinearIntersection3 intersection = segment_triangle_intersection(
            segment,
            {polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
             polyhedron.vertices[face[2]]}
        );
        if(const auto* point = std::get_if<Point3>(&intersection)){
            boundary_events.push_back(parameter(*point));
        }else if(const auto* overlap = std::get_if<Segment3>(&intersection)){
            boundary_events.push_back(parameter(overlap->a));
            boundary_events.push_back(parameter(overlap->b));
        }
    }
    events.insert(events.end(), boundary_events.begin(), boundary_events.end());
    std::sort(events.begin(), events.end());
    const long double parameter_epsilon = 1e-12L;
    events.erase(std::unique(events.begin(), events.end(),
        [parameter_epsilon](long double first, long double second){
            return std::abs(first - second) <= parameter_epsilon;
        }), events.end());
    std::sort(boundary_events.begin(), boundary_events.end());
    boundary_events.erase(std::unique(
        boundary_events.begin(), boundary_events.end(),
        [parameter_epsilon](long double first, long double second){
            return std::abs(first - second) <= parameter_epsilon;
        }
    ), boundary_events.end());

    std::vector<std::pair<long double, long double>> intervals;
    for(std::size_t index = 0; index + 1 < events.size(); ++index){
        const long double left = events[index];
        const long double right = events[index + 1];
        if(right - left <= parameter_epsilon) continue;
        if(!polyhedron_contains_detail::unchecked(
            polyhedron, point_at((left + right) / 2.0L)
        )) continue;
        if(!intervals.empty()
            && std::abs(intervals.back().second - left) <= parameter_epsilon){
            intervals.back().second = right;
        }else{
            intervals.push_back({left, right});
        }
    }
    for(const long double event: boundary_events){
        bool covered = false;
        for(const auto& interval: intervals){
            if(interval.first - parameter_epsilon <= event
                && event <= interval.second + parameter_epsilon){
                covered = true;
                break;
            }
        }
        if(!covered) intervals.push_back({event, event});
    }
    std::sort(intervals.begin(), intervals.end());
    std::vector<Segment3> result;
    result.reserve(intervals.size());
    for(const auto& interval: intervals){
        result.push_back({point_at(interval.first), point_at(interval.second)});
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_SEGMENT_POLYHEDRON_SEGMENT_INTERSECTION_HPP_INCLUDED
