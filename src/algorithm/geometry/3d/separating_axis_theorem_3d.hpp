#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "convex_polyhedron3.hpp"
#include "convex_polyhedron_edges.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "gjk_query_3d.hpp"
#include "is_finite.hpp"
#include "sat_result3.hpp"

namespace sat_3d_detail{

inline long double axis_scale(const Point3& point){
    return std::max({std::abs(point.x), std::abs(point.y), std::abs(point.z)});
}

inline Point3 normalized_axis(Point3 axis){
    const long double scale = axis_scale(axis);
    if(scale == 0.0L || !std::isfinite(scale)) return {};
    axis /= scale;
    const long double length = std::sqrt(dot(axis, axis));
    if(length == 0.0L || !std::isfinite(length)) return {};
    return axis / length;
}

inline bool parallel_axis(
    const Point3& first,
    const Point3& second,
    long double tolerance
){
    const Point3 product = cross(first, second);
    return std::sqrt(dot(product, product)) <= tolerance;
}

inline Point3 canonical_axis(Point3 axis){
    if(axis.x < 0.0L
        || (axis.x == 0.0L && axis.y < 0.0L)
        || (axis.x == 0.0L && axis.y == 0.0L && axis.z < 0.0L)){
        axis = -axis;
    }
    return axis;
}

struct AxisGridKey{
    long double x;
    long double y;
    long double z;

    friend bool operator==(const AxisGridKey&, const AxisGridKey&) = default;
};

struct AxisGridKeyHash{
    std::size_t operator()(const AxisGridKey& key) const{
        std::size_t result = std::hash<long double>{}(key.x);
        const auto combine = [&result](long double value){
            const std::size_t hashed = std::hash<long double>{}(value);
            result ^= hashed + static_cast<std::size_t>(0x9e3779b9U)
                + (result << 6) + (result >> 2);
        };
        combine(key.y);
        combine(key.z);
        return result;
    }
};

class AxisSet{
    long double tolerance_;
    long double grid_width_;
    std::vector<Point3> axes_;
    std::unordered_map<
        AxisGridKey,
        std::vector<std::size_t>,
        AxisGridKeyHash
    > buckets_;

    AxisGridKey key(const Point3& axis) const{
        return {
            std::floor(axis.x / grid_width_),
            std::floor(axis.y / grid_width_),
            std::floor(axis.z / grid_width_)
        };
    }

public:
    explicit AxisSet(long double tolerance):
        tolerance_(tolerance),
        grid_width_(std::max(
            tolerance,
            std::numeric_limits<long double>::min() * 4.0L
        ))
    {}

    void add(Point3 candidate){
        candidate = canonical_axis(normalized_axis(candidate));
        if(axis_scale(candidate) == 0.0L) return;
        if(tolerance_ >= 1.0L){
            if(axes_.empty()) axes_.push_back(candidate);
            return;
        }
        const AxisGridKey center = key(candidate);
        const auto has_parallel_near = [&](const AxisGridKey& search_center){
            for(int dx = -2; dx <= 2; ++dx){
                for(int dy = -2; dy <= 2; ++dy){
                    for(int dz = -2; dz <= 2; ++dz){
                        const AxisGridKey nearby{
                            search_center.x + static_cast<long double>(dx),
                            search_center.y + static_cast<long double>(dy),
                            search_center.z + static_cast<long double>(dz)
                        };
                        const auto iterator = buckets_.find(nearby);
                        if(iterator == buckets_.end()) continue;
                        for(const std::size_t index: iterator->second){
                            if(parallel_axis(
                                axes_[index], candidate, tolerance_
                            )) return true;
                        }
                    }
                }
            }
            return false;
        };
        if(has_parallel_near(center)
            || has_parallel_near(key(-candidate))) return;
        const std::size_t index = axes_.size();
        axes_.push_back(candidate);
        buckets_[center].push_back(index);
    }

    const std::vector<Point3>& values() const{ return axes_; }
};

inline void add_face_axes(
    const ConvexPolyhedron3& polyhedron,
    AxisSet& axes
){
    for(const auto& face: polyhedron.faces){
        for(std::size_t index: face){
            if(index >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("SAT face index");
            }
        }
        const Point3 first =
            polyhedron.vertices[face[1]] - polyhedron.vertices[face[0]];
        const Point3 second =
            polyhedron.vertices[face[2]] - polyhedron.vertices[face[0]];
        const long double scale = std::max(axis_scale(first), axis_scale(second));
        if(scale == 0.0L) continue;
        axes.add(cross(first / scale, second / scale));
    }
}

inline Point3 vertex_average(const ConvexPolyhedron3& polyhedron){
    long double scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("SAT requires finite vertices");
        }
        scale = std::max(scale, axis_scale(point));
    }
    if(scale == 0.0L) return {};
    Point3 result{};
    for(const Point3& point: polyhedron.vertices) result += point / scale;
    return result * (scale / static_cast<long double>(polyhedron.vertices.size()));
}

inline std::pair<long double, long double> projection(
    const ConvexPolyhedron3& polyhedron,
    const Point3& axis
){
    long double minimum = dot(polyhedron.vertices.front(), axis);
    long double maximum = minimum;
    for(const Point3& point: polyhedron.vertices){
        const long double value = dot(point, axis);
        minimum = std::min(minimum, value);
        maximum = std::max(maximum, value);
    }
    return {minimum, maximum};
}

inline SATResult3 separating_axis_theorem_core(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance
){
    if(first.vertices.empty() || second.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("SAT requires nonempty convex polyhedra");
    }
    if(first.affine_dimension != 3 || second.affine_dimension != 3)[[unlikely]]{
        throw std::invalid_argument("3D SAT requires full-dimensional polyhedra");
    }
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("SAT tolerance must be finite and positive");
    }

    AxisSet axis_set(tolerance);
    add_face_axes(first, axis_set);
    add_face_axes(second, axis_set);
    const auto first_edges = convex_polyhedron_edges(first);
    const auto second_edges = convex_polyhedron_edges(second);
    for(const auto& first_edge: first_edges){
        Point3 first_direction = first.vertices[first_edge[1]]
            - first.vertices[first_edge[0]];
        const long double first_scale = axis_scale(first_direction);
        if(first_scale == 0.0L) continue;
        first_direction /= first_scale;
        for(const auto& second_edge: second_edges){
            Point3 second_direction = second.vertices[second_edge[1]]
                - second.vertices[second_edge[0]];
            const long double second_scale = axis_scale(second_direction);
            if(second_scale == 0.0L) continue;
            second_direction /= second_scale;
            axis_set.add(cross(first_direction, second_direction));
        }
    }
    const std::vector<Point3>& axes = axis_set.values();
    if(axes.empty())[[unlikely]]{
        throw std::domain_error("SAT found no nonzero candidate axis");
    }

    const Point3 center_direction = vertex_average(second) - vertex_average(first);
    SATResult3 result;
    result.intersects = true;
    result.penetration_depth = std::numeric_limits<long double>::infinity();
    long double largest_gap = -std::numeric_limits<long double>::infinity();
    Point3 separation_axis{};
    for(Point3 axis: axes){
        ++result.tested_axes;
        if(dot(axis, center_direction) < 0.0L) axis = -axis;
        const auto [first_minimum, first_maximum] = projection(first, axis);
        const auto [second_minimum, second_maximum] = projection(second, axis);
        const long double forward_gap = second_minimum - first_maximum;
        const long double backward_gap = first_minimum - second_maximum;
        const long double gap = std::max(forward_gap, backward_gap);
        if(gap > tolerance){
            result.intersects = false;
            if(gap > largest_gap){
                largest_gap = gap;
                separation_axis = forward_gap >= backward_gap ? axis : -axis;
            }
            continue;
        }
        const long double overlap = std::min(first_maximum, second_maximum)
            - std::max(first_minimum, second_minimum);
        if(overlap < result.penetration_depth){
            result.penetration_depth = std::max(0.0L, overlap);
            result.axis = axis;
        }
    }
    if(!result.intersects){
        result.separation = largest_gap;
        result.penetration_depth = 0.0L;
        result.axis = separation_axis;
    }
    return result;
}

inline SATResult3 restore_result(
    SATResult3 result,
    const gjk_3d_detail::CollisionNormalization3& normalization
){
    result.separation = gjk_3d_detail::restore_length(
        result.separation, normalization
    );
    result.penetration_depth = gjk_3d_detail::restore_length(
        result.penetration_depth, normalization
    );
    result.axis = gjk_3d_detail::finite_unit_normal(result.axis);
    return result;
}

}  // namespace sat_3d_detail

inline SATResult3 separating_axis_theorem_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L
){
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("SAT tolerance must be finite and positive");
    }
    const gjk_3d_detail::CollisionNormalization3 normalization =
        gjk_3d_detail::normalize_pair(first, second);
    return sat_3d_detail::restore_result(
        sat_3d_detail::separating_axis_theorem_core(
            normalization.first, normalization.second, tolerance
        ),
        normalization
    );
}
