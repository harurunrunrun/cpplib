#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_SEGMENT_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_SEGMENT_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_SEGMENT_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../detail/polyhedron/convex_polyhedron_query_hierarchy_3d_detail.hpp"
#include "../../predicate/segment_point/on_segment.hpp"
#include "../segment_segment/segment_segment_intersection.hpp"
#include "../segment_triangle/segment_triangle_intersection.hpp"

namespace convex_polyhedron_query_hierarchy_3d_segment_detail{

using Exact = convex_polyhedron_query_hierarchy_3d_detail::Exact;

struct ExactRatio{
    Exact numerator{};
    Exact denominator = geometry3d_adaptive_detail::exact_dyadic(1.0L);
};

inline int compare_ratio(const ExactRatio& left, const ExactRatio& right){
    using geometry3d_adaptive_detail::multiply;
    using geometry3d_adaptive_detail::sign;
    using geometry3d_adaptive_detail::subtract;
    return sign(subtract(
        multiply(left.numerator, right.denominator),
        multiply(right.numerator, left.denominator)
    ));
}

inline bool segment_intersects_box(
    const convex_polyhedron_query_hierarchy_3d_detail::Box3& box,
    const Segment3& segment
){
    using geometry3d_adaptive_detail::exact_dyadic;
    using geometry3d_adaptive_detail::negate;
    using geometry3d_adaptive_detail::sign;
    using geometry3d_adaptive_detail::subtract;

    const auto first =
        convex_polyhedron_query_hierarchy_3d_detail::exact_point(segment.a);
    const auto second =
        convex_polyhedron_query_hierarchy_3d_detail::exact_point(segment.b);
    ExactRatio lower{exact_dyadic(0.0L), exact_dyadic(1.0L)};
    ExactRatio upper{exact_dyadic(1.0L), exact_dyadic(1.0L)};
    for(std::size_t axis = 0; axis < 3; ++axis){
        const Exact direction = subtract(second[axis], first[axis]);
        const int direction_sign = sign(direction);
        if(direction_sign == 0){
            if(convex_polyhedron_query_hierarchy_3d_detail::compare(
                first[axis], box.exact.minimum[axis]
            ) < 0 || convex_polyhedron_query_hierarchy_3d_detail::compare(
                first[axis], box.exact.maximum[axis]
            ) > 0) return false;
            continue;
        }

        ExactRatio axis_lower;
        ExactRatio axis_upper;
        if(direction_sign > 0){
            axis_lower = {
                subtract(box.exact.minimum[axis], first[axis]), direction
            };
            axis_upper = {
                subtract(box.exact.maximum[axis], first[axis]), direction
            };
        }else{
            const Exact positive_direction = negate(direction);
            axis_lower = {
                subtract(first[axis], box.exact.maximum[axis]),
                positive_direction,
            };
            axis_upper = {
                subtract(first[axis], box.exact.minimum[axis]),
                positive_direction,
            };
        }
        if(compare_ratio(lower, axis_lower) < 0) lower = axis_lower;
        if(compare_ratio(axis_upper, upper) < 0) upper = axis_upper;
        if(compare_ratio(upper, lower) < 0) return false;
    }
    return true;
}

inline long double parameter(const Segment3& segment, const Point3& point){
    const std::array<long double, 3> first{
        segment.a.x, segment.a.y, segment.a.z
    };
    const std::array<long double, 3> second{
        segment.b.x, segment.b.y, segment.b.z
    };
    const std::array<long double, 3> query{point.x, point.y, point.z};
    std::size_t best_axis = 0;
    long double best_difference = 0.0L;
    for(std::size_t axis = 0; axis < 3; ++axis){
        const long double scale = std::max({
            1.0L, std::abs(first[axis]), std::abs(second[axis])
        });
        const long double difference =
            second[axis] / scale - first[axis] / scale;
        if(std::abs(difference) > std::abs(best_difference)){
            best_axis = axis;
            best_difference = difference;
        }
    }
    if(best_difference == 0.0L)[[unlikely]] return 0.0L;
    const long double scale = std::max({
        1.0L, std::abs(first[best_axis]),
        std::abs(second[best_axis]), std::abs(query[best_axis])
    });
    return (query[best_axis] / scale - first[best_axis] / scale)
        / (second[best_axis] / scale - first[best_axis] / scale);
}

inline Point3 interpolate(const Segment3& segment, long double value){
    return {
        std::lerp(segment.a.x, segment.b.x, value),
        std::lerp(segment.a.y, segment.b.y, value),
        std::lerp(segment.a.z, segment.b.z, value),
    };
}

}  // namespace convex_polyhedron_query_hierarchy_3d_segment_detail

inline std::optional<Segment3>
ConvexPolyhedronQueryHierarchy3D::segment_intersection(
    const Segment3& segment
) const{
    using namespace convex_polyhedron_query_hierarchy_3d_segment_detail;
    if(!geometry3d_is_finite(segment.a)
        || !geometry3d_is_finite(segment.b))[[unlikely]]{
        throw std::invalid_argument("non-finite segment intersection query");
    }
    if(polyhedron_.affine_dimension == 0){
        if(!on_segment(segment, polyhedron_.vertices.front())){
            return std::nullopt;
        }
        return Segment3{
            polyhedron_.vertices.front(), polyhedron_.vertices.front()
        };
    }
    if(polyhedron_.affine_dimension == 1){
        const LinearIntersection3 intersection = segment_segment_intersection(
            segment,
            {polyhedron_.vertices.front(), polyhedron_.vertices.back()}
        );
        if(const Point3* point = std::get_if<Point3>(&intersection)){
            return Segment3{*point, *point};
        }
        if(const Segment3* overlap = std::get_if<Segment3>(&intersection)){
            return *overlap;
        }
        return std::nullopt;
    }
    if(segment.a == segment.b){
        return contains(segment.a) ? std::optional<Segment3>{segment}
            : std::nullopt;
    }

    long double lower = std::numeric_limits<long double>::infinity();
    long double upper = -std::numeric_limits<long double>::infinity();
    const auto add = [&](const Point3& point){
        const long double value = std::clamp(
            parameter(segment, point), 0.0L, 1.0L
        );
        lower = std::min(lower, value);
        upper = std::max(upper, value);
    };
    if(contains(segment.a)) add(segment.a);
    if(contains(segment.b)) add(segment.b);

    std::vector<std::size_t> stack{0};
    while(!stack.empty()){
        const std::size_t node_index = stack.back();
        stack.pop_back();
        const FaceNode& node = face_nodes_[node_index];
        if(!segment_intersects_box(node.box, segment)) continue;
        if(!node.leaf()){
            stack.push_back(node.left);
            stack.push_back(node.right);
            continue;
        }
        for(std::size_t position = node.begin; position < node.end; ++position){
            const LinearIntersection3 intersection =
                segment_triangle_intersection(
                    segment,
                    triangle(polyhedron_, closest_face_order_[position])
                );
            if(const Point3* point = std::get_if<Point3>(&intersection)){
                add(*point);
            }else if(const Segment3* overlap =
                std::get_if<Segment3>(&intersection)){
                add(overlap->a);
                add(overlap->b);
            }
        }
    }
    if(lower > upper) return std::nullopt;
    return Segment3{interpolate(segment, lower), interpolate(segment, upper)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POLYHEDRON_SEGMENT_CONVEX_POLYHEDRON_QUERY_HIERARCHY_3D_SEGMENT_INTERSECTION_HPP_INCLUDED
