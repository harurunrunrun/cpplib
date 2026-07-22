#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CONVEX_POLYHEDRON_INTERSECTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CONVEX_POLYHEDRON_INTERSECTS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_orient3d.hpp"
#include "../query/convex_polyhedron_edges.hpp"
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

using ExactIntegerVector = std::array<ExactInteger, 3>;

struct ExactHalfspace3{
    ExactIntegerVector normal{};
    ExactInteger bound = 0;
};

struct ExactHomogeneousPoint3{
    ExactIntegerVector numerator{};
    ExactInteger denominator = 1;
};

inline ExactInteger integer_dot(
    const ExactIntegerVector& first,
    const ExactIntegerVector& second
){
    return first[0] * second[0]
        + first[1] * second[1]
        + first[2] * second[2];
}

inline ExactIntegerVector integer_cross(
    const ExactIntegerVector& first,
    const ExactIntegerVector& second
){
    return {
        first[1] * second[2] - first[2] * second[1],
        first[2] * second[0] - first[0] * second[2],
        first[0] * second[1] - first[1] * second[0],
    };
}

inline ExactHalfspace3 integer_halfspace(
    const ExactVector& normal,
    const ExactScalar& bound
){
    const std::array<ExactScalar, 4> values{
        normal[0], normal[1], normal[2], bound
    };
    bool has_nonzero = false;
    int common_exponent = 0;
    for(const ExactScalar& value: values){
        if(value.significand == 0) continue;
        if(!has_nonzero || value.exponent < common_exponent){
            common_exponent = value.exponent;
            has_nonzero = true;
        }
    }
    ExactHalfspace3 result;
    if(!has_nonzero) return result;
    const auto scaled = [&](const ExactScalar& value){
        if(value.significand == 0) return ExactInteger{};
        return value.significand << static_cast<unsigned>(
            value.exponent - common_exponent
        );
    };
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result.normal[coordinate] = scaled(normal[coordinate]);
    }
    result.bound = scaled(bound);
    return result;
}

inline void append_exact_halfspaces(
    const ConvexPolyhedron3& polyhedron,
    std::vector<ExactHalfspace3>& halfspaces
){
    using geometry3d_adaptive_detail::add;
    using geometry3d_adaptive_detail::multiply;
    using geometry3d_adaptive_detail::negate;
    using geometry3d_adaptive_detail::sign;
    using geometry3d_adaptive_detail::subtract;

    if(polyhedron.affine_dimension != 3
        || polyhedron.vertices.empty()
        || polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument(
            "exact intersection requires a full-dimensional polyhedron"
        );
    }

    std::vector<ExactVector> vertices;
    vertices.reserve(polyhedron.vertices.size());
    ExactVector sum{};
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "exact intersection requires finite vertices"
            );
        }
        vertices.push_back(exact_point(point));
        for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
            sum[coordinate] = add(
                sum[coordinate], vertices.back()[coordinate]
            );
        }
    }

    const ExactScalar vertex_count{
        ExactInteger(polyhedron.vertices.size()), 0
    };
    halfspaces.reserve(halfspaces.size() + polyhedron.faces.size());
    for(const auto& face: polyhedron.faces){
        for(const std::size_t index: face){
            if(index >= vertices.size())[[unlikely]]{
                throw std::out_of_range("exact intersection face index");
            }
        }
        ExactVector normal = exact_cross(
            exact_difference(vertices[face[1]], vertices[face[0]]),
            exact_difference(vertices[face[2]], vertices[face[0]])
        );
        if(exact_zero(normal))[[unlikely]]{
            throw std::domain_error(
                "exact intersection has a degenerate face"
            );
        }
        ExactScalar bound = exact_dot(normal, vertices[face[0]]);
        const int center_side = sign(subtract(
            exact_dot(normal, sum),
            multiply(bound, vertex_count)
        ));
        if(center_side == 0)[[unlikely]]{
            throw std::domain_error(
                "exact intersection face does not bound a spatial polyhedron"
            );
        }
        if(center_side > 0){
            for(ExactScalar& coordinate: normal){
                coordinate = negate(std::move(coordinate));
            }
            bound = negate(std::move(bound));
        }
        halfspaces.push_back(integer_halfspace(normal, bound));
    }
}

inline constexpr std::size_t exact_bounding_halfspace_count = 6;

inline void append_exact_bounding_box(
    const ConvexPolyhedron3& polyhedron,
    std::vector<ExactHalfspace3>& halfspaces
){
    using geometry3d_adaptive_detail::exact_dyadic;
    using geometry3d_adaptive_detail::negate;
    if(polyhedron.affine_dimension != 3
        || polyhedron.vertices.empty()
        || polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument(
            "exact intersection requires a full-dimensional polyhedron"
        );
    }
    std::array<long double, 3> minimum{
        polyhedron.vertices.front().x,
        polyhedron.vertices.front().y,
        polyhedron.vertices.front().z,
    };
    std::array<long double, 3> maximum = minimum;
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "exact intersection requires finite vertices"
            );
        }
        const std::array<long double, 3> coordinate{
            point.x, point.y, point.z
        };
        for(std::size_t axis = 0; axis < 3; ++axis){
            minimum[axis] = std::min(minimum[axis], coordinate[axis]);
            maximum[axis] = std::max(maximum[axis], coordinate[axis]);
        }
    }
    halfspaces.reserve(
        halfspaces.size() + exact_bounding_halfspace_count
    );
    for(std::size_t axis = 0; axis < 3; ++axis){
        ExactVector normal{};
        normal[axis] = exact_dyadic(1.0L);
        halfspaces.push_back(integer_halfspace(
            normal, exact_dyadic(maximum[axis])
        ));
        normal[axis] = negate(normal[axis]);
        halfspaces.push_back(integer_halfspace(
            normal, negate(exact_dyadic(minimum[axis]))
        ));
    }
}

inline std::vector<ExactHalfspace3> combined_exact_halfspaces(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    std::vector<ExactHalfspace3> result;
    result.reserve(
        exact_bounding_halfspace_count
        + first.faces.size() + second.faces.size()
    );
    append_exact_bounding_box(first, result);
    append_exact_halfspaces(first, result);
    append_exact_halfspaces(second, result);
    return result;
}

inline bool satisfies(
    const ExactHalfspace3& halfspace,
    const ExactHomogeneousPoint3& point
){
    return integer_dot(halfspace.normal, point.numerator)
        - halfspace.bound * point.denominator <= 0;
}

inline std::optional<ExactHomogeneousPoint3>
minimum_norm_point_on_plane(const ExactHalfspace3& plane){
    const ExactInteger denominator =
        integer_dot(plane.normal, plane.normal);
    if(denominator == 0) return std::nullopt;
    ExactHomogeneousPoint3 result;
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result.numerator[coordinate] =
            plane.bound * plane.normal[coordinate];
    }
    result.denominator = denominator;
    return result;
}

inline std::optional<ExactHomogeneousPoint3>
minimum_norm_point_on_line(
    const ExactHalfspace3& first,
    const ExactHalfspace3& second
){
    const ExactInteger first_squared =
        integer_dot(first.normal, first.normal);
    const ExactInteger second_squared =
        integer_dot(second.normal, second.normal);
    const ExactInteger product =
        integer_dot(first.normal, second.normal);
    const ExactInteger denominator =
        first_squared * second_squared - product * product;
    if(denominator == 0) return std::nullopt;

    const ExactInteger first_multiplier =
        first.bound * second_squared - second.bound * product;
    const ExactInteger second_multiplier =
        second.bound * first_squared - first.bound * product;
    ExactHomogeneousPoint3 result;
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result.numerator[coordinate] =
            first_multiplier * first.normal[coordinate]
            + second_multiplier * second.normal[coordinate];
    }
    result.denominator = denominator;
    return result;
}

inline std::optional<ExactHomogeneousPoint3>
intersection_point(
    const ExactHalfspace3& first,
    const ExactHalfspace3& second,
    const ExactHalfspace3& third
){
    const ExactIntegerVector second_third =
        integer_cross(second.normal, third.normal);
    ExactInteger denominator =
        integer_dot(first.normal, second_third);
    if(denominator == 0) return std::nullopt;

    const ExactIntegerVector third_first =
        integer_cross(third.normal, first.normal);
    const ExactIntegerVector first_second =
        integer_cross(first.normal, second.normal);
    ExactHomogeneousPoint3 result;
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result.numerator[coordinate] =
            first.bound * second_third[coordinate]
            + second.bound * third_first[coordinate]
            + third.bound * first_second[coordinate];
    }
    if(denominator < 0){
        denominator = -denominator;
        for(ExactInteger& coordinate: result.numerator){
            coordinate = -coordinate;
        }
    }
    result.denominator = std::move(denominator);
    return result;
}

struct ExactRational{
    ExactInteger numerator = 0;
    ExactInteger denominator = 1;
};

inline bool rational_less(
    const ExactRational& first,
    const ExactRational& second
){
    return first.numerator * second.denominator
        < second.numerator * first.denominator;
}

inline std::optional<ExactHomogeneousPoint3>
minimum_norm_feasible_on_line(
    const ExactHalfspace3& first_boundary,
    const ExactHalfspace3& second_boundary,
    const std::vector<ExactHalfspace3>& halfspaces,
    const std::vector<std::size_t>& order,
    std::size_t prefix
){
    const auto closest = minimum_norm_point_on_line(
        first_boundary, second_boundary
    );
    if(!closest) return std::nullopt;
    const ExactIntegerVector direction = integer_cross(
        first_boundary.normal, second_boundary.normal
    );

    std::optional<ExactRational> lower;
    std::optional<ExactRational> upper;
    for(std::size_t position = 0; position < prefix; ++position){
        const ExactHalfspace3& constraint =
            halfspaces[order[position]];
        const ExactInteger slope = closest->denominator
            * integer_dot(constraint.normal, direction);
        const ExactInteger bound = constraint.bound * closest->denominator
            - integer_dot(constraint.normal, closest->numerator);
        if(slope == 0){
            if(bound < 0) return std::nullopt;
            continue;
        }
        ExactRational endpoint;
        if(slope > 0){
            endpoint = {bound, slope};
            if(!upper || rational_less(endpoint, *upper)){
                upper = std::move(endpoint);
            }
        }else{
            endpoint = {-bound, -slope};
            if(!lower || rational_less(*lower, endpoint)){
                lower = std::move(endpoint);
            }
        }
    }
    if(lower && upper && rational_less(*upper, *lower)){
        return std::nullopt;
    }

    ExactRational parameter;
    if(lower && lower->numerator > 0){
        parameter = *lower;
    }else if(upper && upper->numerator < 0){
        parameter = *upper;
    }
    ExactHomogeneousPoint3 result;
    result.denominator =
        closest->denominator * parameter.denominator;
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result.numerator[coordinate] =
            closest->numerator[coordinate] * parameter.denominator
            + parameter.numerator * closest->denominator
                * direction[coordinate];
    }
    return result;
}

inline std::optional<ExactHomogeneousPoint3>
minimum_norm_feasible_on_plane(
    const ExactHalfspace3& boundary,
    const std::vector<ExactHalfspace3>& halfspaces,
    const std::vector<std::size_t>& order,
    std::size_t prefix
){
    auto candidate = minimum_norm_point_on_plane(boundary);
    if(!candidate) return std::nullopt;
    for(std::size_t position = 0; position < prefix; ++position){
        const ExactHalfspace3& constraint =
            halfspaces[order[position]];
        if(satisfies(constraint, *candidate)) continue;
        candidate = minimum_norm_feasible_on_line(
            boundary, constraint, halfspaces, order, position
        );
        if(!candidate) return std::nullopt;
    }
    return candidate;
}

inline bool randomized_exact_feasible(
    const std::vector<ExactHalfspace3>& halfspaces,
    std::mt19937_64& random_engine
){
    std::vector<std::size_t> order(halfspaces.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    if(order.size() < exact_bounding_halfspace_count)[[unlikely]]{
        throw std::logic_error("missing exact intersection bounding box");
    }
    std::shuffle(
        order.begin() + exact_bounding_halfspace_count,
        order.end(), random_engine
    );

    ExactHomogeneousPoint3 candidate;
    for(std::size_t position = 0; position < order.size(); ++position){
        const ExactHalfspace3& constraint =
            halfspaces[order[position]];
        if(satisfies(constraint, candidate)) continue;
        const auto replacement = minimum_norm_feasible_on_plane(
            constraint, halfspaces, order, position
        );
        if(!replacement) return false;
        candidate = *replacement;
    }
    return true;
}

inline std::mt19937_64 independent_random_engine(){
    std::random_device source;
    std::array<std::uint32_t, 8> seed_data{};
    for(std::uint32_t& value: seed_data){
        value = static_cast<std::uint32_t>(source());
    }
    std::seed_seq seed(seed_data.begin(), seed_data.end());
    return std::mt19937_64(seed);
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

inline bool convex_polyhedron_intersects_exact_with_seed(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::uint64_t random_seed
){
    using namespace convex_polyhedron_intersects_detail;
    std::vector<ExactHalfspace3> halfspaces =
        combined_exact_halfspaces(first, second);
    std::mt19937_64 random_engine(random_seed);
    return randomized_exact_feasible(halfspaces, random_engine);
}

inline bool convex_polyhedron_intersects_exact(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    using namespace convex_polyhedron_intersects_detail;
    std::vector<ExactHalfspace3> halfspaces =
        combined_exact_halfspaces(first, second);
    std::mt19937_64 random_engine = independent_random_engine();
    return randomized_exact_feasible(halfspaces, random_engine);
}

inline bool convex_polyhedron_intersects_exact_sat(
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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_CONVEX_POLYHEDRON_INTERSECTS_HPP_INCLUDED
