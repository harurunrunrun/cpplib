#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "convex_polyhedron_edges.hpp"
#include "gjk_intersects_3d.hpp"
#include "is_finite.hpp"

namespace convex_polyhedron_intersects_detail{

using ExactScalar = geometry3d_adaptive_detail::ExactDyadic;
using ExactVector = std::array<ExactScalar, 3>;

inline ExactVector exact_point(const Point3& point){
    using geometry3d_adaptive_detail::exact_dyadic;
    return {
        exact_dyadic(point.x),
        exact_dyadic(point.y),
        exact_dyadic(point.z),
    };
}

inline ExactVector exact_difference(
    const ExactVector& first,
    const ExactVector& second
){
    using geometry3d_adaptive_detail::subtract;
    return {
        subtract(first[0], second[0]),
        subtract(first[1], second[1]),
        subtract(first[2], second[2]),
    };
}

inline ExactVector exact_cross(
    const ExactVector& first,
    const ExactVector& second
){
    using geometry3d_adaptive_detail::multiply;
    using geometry3d_adaptive_detail::subtract;
    return {
        subtract(multiply(first[1], second[2]),
                 multiply(first[2], second[1])),
        subtract(multiply(first[2], second[0]),
                 multiply(first[0], second[2])),
        subtract(multiply(first[0], second[1]),
                 multiply(first[1], second[0])),
    };
}

inline ExactScalar exact_dot(
    const ExactVector& first,
    const ExactVector& second
){
    using geometry3d_adaptive_detail::add;
    using geometry3d_adaptive_detail::multiply;
    return add(
        add(multiply(first[0], second[0]),
            multiply(first[1], second[1])),
        multiply(first[2], second[2])
    );
}

inline bool exact_zero(const ExactVector& vector){
    using geometry3d_adaptive_detail::sign;
    return sign(vector[0]) == 0
        && sign(vector[1]) == 0
        && sign(vector[2]) == 0;
}

struct ExactPolyhedron{
    std::vector<ExactVector> vertices;
    std::vector<std::array<std::size_t, 2>> edges;
};

inline ExactPolyhedron exact_polyhedron(
    const ConvexPolyhedron3& polyhedron
){
    if(polyhedron.affine_dimension != 3
        || polyhedron.vertices.empty()
        || polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument(
            "exact intersection requires a full-dimensional polyhedron"
        );
    }
    ExactPolyhedron result;
    result.vertices.reserve(polyhedron.vertices.size());
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "exact intersection requires finite vertices"
            );
        }
        result.vertices.push_back(exact_point(point));
    }
    for(const auto& face: polyhedron.faces){
        for(const std::size_t index: face){
            if(index >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range(
                    "exact intersection face index"
                );
            }
        }
    }
    result.edges = convex_polyhedron_edges(polyhedron);
    if(result.edges.empty())[[unlikely]]{
        throw std::domain_error(
            "exact intersection found no polyhedron edge"
        );
    }
    return result;
}

inline bool exact_axis_separates(
    const ExactVector& axis,
    const std::vector<ExactVector>& first,
    const std::vector<ExactVector>& second
){
    using geometry3d_adaptive_detail::sign;
    using geometry3d_adaptive_detail::subtract;
    if(exact_zero(axis)) return false;
    const ExactVector& origin = first.front();
    auto projection = [&](const std::vector<ExactVector>& vertices){
        ExactScalar minimum = exact_dot(
            axis, exact_difference(vertices.front(), origin)
        );
        ExactScalar maximum = minimum;
        for(std::size_t index = 1; index < vertices.size(); ++index){
            ExactScalar value = exact_dot(
                axis, exact_difference(vertices[index], origin)
            );
            if(sign(subtract(value, minimum)) < 0) minimum = value;
            if(sign(subtract(value, maximum)) > 0) maximum = std::move(value);
        }
        return std::pair<ExactScalar, ExactScalar>{
            std::move(minimum), std::move(maximum)
        };
    };
    const auto [first_minimum, first_maximum] = projection(first);
    const auto [second_minimum, second_maximum] = projection(second);
    return sign(subtract(first_maximum, second_minimum)) < 0
        || sign(subtract(second_maximum, first_minimum)) < 0;
}

inline bool face_axis_separates(
    const ConvexPolyhedron3& source,
    const ExactPolyhedron& exact_source,
    const ExactPolyhedron& first,
    const ExactPolyhedron& second
){
    for(const auto& face: source.faces){
        const ExactVector first_direction = exact_difference(
            exact_source.vertices[face[1]], exact_source.vertices[face[0]]
        );
        const ExactVector second_direction = exact_difference(
            exact_source.vertices[face[2]], exact_source.vertices[face[0]]
        );
        if(exact_axis_separates(
            exact_cross(first_direction, second_direction),
            first.vertices, second.vertices
        )) return true;
    }
    return false;
}

}  // namespace convex_polyhedron_intersects_detail

inline bool convex_polyhedron_intersects(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    return gjk_intersects_3d(
        first, second, tolerance, max_iterations
    );
}

inline bool convex_polyhedron_intersects_exact(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    using namespace convex_polyhedron_intersects_detail;
    const ExactPolyhedron exact_first = exact_polyhedron(first);
    const ExactPolyhedron exact_second = exact_polyhedron(second);
    if(face_axis_separates(
        first, exact_first, exact_first, exact_second
    ) || face_axis_separates(
        second, exact_second, exact_first, exact_second
    )) return false;

    for(const auto& first_edge: exact_first.edges){
        const ExactVector first_direction = exact_difference(
            exact_first.vertices[first_edge[1]],
            exact_first.vertices[first_edge[0]]
        );
        for(const auto& second_edge: exact_second.edges){
            const ExactVector second_direction = exact_difference(
                exact_second.vertices[second_edge[1]],
                exact_second.vertices[second_edge[0]]
            );
            if(exact_axis_separates(
                exact_cross(first_direction, second_direction),
                exact_first.vertices, exact_second.vertices
            )) return false;
        }
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_HPP_INCLUDED
