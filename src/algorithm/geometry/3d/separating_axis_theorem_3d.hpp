#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "convex_polyhedron3.hpp"
#include "convex_polyhedron_edges.hpp"
#include "cross.hpp"
#include "dot.hpp"
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

inline void add_axis(
    std::vector<Point3>& axes,
    Point3 candidate,
    long double tolerance
){
    candidate = normalized_axis(candidate);
    if(axis_scale(candidate) == 0.0L) return;
    for(const Point3& axis: axes){
        if(parallel_axis(axis, candidate, tolerance)) return;
    }
    axes.push_back(candidate);
}

inline std::vector<Point3> face_axes(
    const ConvexPolyhedron3& polyhedron,
    long double tolerance
){
    std::vector<Point3> axes;
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
        add_axis(axes, cross(first / scale, second / scale), tolerance);
    }
    return axes;
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

}  // namespace sat_3d_detail

inline SATResult3 separating_axis_theorem_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L
){
    using namespace sat_3d_detail;
    if(first.vertices.empty() || second.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("SAT requires nonempty convex polyhedra");
    }
    if(first.affine_dimension != 3 || second.affine_dimension != 3)[[unlikely]]{
        throw std::invalid_argument("3D SAT requires full-dimensional polyhedra");
    }
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("SAT tolerance must be finite and positive");
    }

    std::vector<Point3> axes = face_axes(first, tolerance);
    const std::vector<Point3> second_face_axes = face_axes(second, tolerance);
    for(const Point3& axis: second_face_axes) add_axis(axes, axis, tolerance);
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
            add_axis(
                axes, cross(first_direction, second_direction), tolerance
            );
        }
    }
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
