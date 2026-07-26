#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GJK_QUERY_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GJK_QUERY_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "convex_polyhedron3.hpp"
#include "dot.hpp"
#include "gjk_result3.hpp"
#include "is_finite.hpp"

namespace gjk_3d_detail{

struct ClosestSimplex3{
    Point3 point{};
    Point3 point_on_first{};
    Point3 point_on_second{};
    std::vector<GJKSupportPoint3> simplex;
    std::vector<long double> weights;
};

struct CollisionNormalization3{
    ConvexPolyhedron3 first;
    ConvexPolyhedron3 second;
    long double outer_scale = 1.0L;
    long double inner_scale = 1.0L;
    Point3 origin{};
};

inline long double max_coordinate(const Point3& point){
    return std::max({std::abs(point.x), std::abs(point.y), std::abs(point.z)});
}

inline long double stable_dot(const Point3& first, const Point3& second){
    const long double first_scale = max_coordinate(first);
    const long double second_scale = max_coordinate(second);
    if(first_scale == 0.0L || second_scale == 0.0L) return 0.0L;
    const long double normalized = dot(
        first / first_scale, second / second_scale
    );
    if(normalized == 0.0L) return 0.0L;
    int value_exponent = 0;
    int first_exponent = 0;
    int second_exponent = 0;
    const long double mantissa =
        std::frexp(normalized, &value_exponent)
        * std::frexp(first_scale, &first_exponent)
        * std::frexp(second_scale, &second_exponent);
    const long double result = std::scalbn(
        mantissa, value_exponent + first_exponent + second_exponent
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("3D scalar result is not representable");
    }
    return result;
}

inline long double stable_squared_norm(const Point3& point){
    const long double scale = max_coordinate(point);
    if(scale == 0.0L) return 0.0L;
    const Point3 normalized = point / scale;
    const long double normalized_squared = dot(normalized, normalized);
    int value_exponent = 0;
    int first_exponent = 0;
    int second_exponent = 0;
    const long double mantissa =
        std::frexp(normalized_squared, &value_exponent)
        * std::frexp(scale, &first_exponent)
        * std::frexp(scale, &second_exponent);
    const long double result = std::scalbn(
        mantissa, value_exponent + first_exponent + second_exponent
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error(
            "3D squared norm is not representable"
        );
    }
    return result;
}

inline long double stable_scaled_squared(
    long double normalized_squared,
    long double scale
){
    if(normalized_squared == 0.0L || scale == 0.0L) return 0.0L;
    int value_exponent = 0;
    int first_exponent = 0;
    int second_exponent = 0;
    const long double mantissa =
        std::frexp(normalized_squared, &value_exponent)
        * std::frexp(scale, &first_exponent)
        * std::frexp(scale, &second_exponent);
    const long double result = std::scalbn(
        mantissa, value_exponent + first_exponent + second_exponent
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error(
            "3D squared norm is not representable"
        );
    }
    return result;
}

inline bool solve_linear_system(
    std::array<std::array<long double, 5>, 5> matrix,
    std::array<long double, 5> right,
    std::size_t size,
    std::array<long double, 5>& answer
){
    for(std::size_t column = 0; column < size; ++column){
        std::size_t pivot = column;
        for(std::size_t row = column + 1; row < size; ++row){
            if(std::abs(matrix[row][column]) >
                std::abs(matrix[pivot][column])){
                pivot = row;
            }
        }
        long double row_scale = 0.0L;
        for(std::size_t index = column; index < size; ++index){
            row_scale = std::max(row_scale, std::abs(matrix[pivot][index]));
        }
        if(row_scale == 0.0L ||
            std::abs(matrix[pivot][column]) <=
                64.0L * std::numeric_limits<long double>::epsilon()
                    * row_scale){
            return false;
        }
        std::swap(matrix[pivot], matrix[column]);
        std::swap(right[pivot], right[column]);
        const long double divisor = matrix[column][column];
        for(std::size_t index = column; index < size; ++index){
            matrix[column][index] /= divisor;
        }
        right[column] /= divisor;
        for(std::size_t row = 0; row < size; ++row){
            if(row == column) continue;
            const long double factor = matrix[row][column];
            for(std::size_t index = column; index < size; ++index){
                matrix[row][index] -= factor * matrix[column][index];
            }
            right[row] -= factor * right[column];
        }
    }
    answer = right;
    return true;
}

inline ClosestSimplex3 closest_simplex(
    const std::vector<GJKSupportPoint3>& simplex,
    long double tolerance
){
    const std::size_t count = simplex.size();
    ClosestSimplex3 best;
    long double best_squared = std::numeric_limits<long double>::infinity();
    for(unsigned mask = 1; mask < (1U << count); ++mask){
        std::array<std::size_t, 4> indices{};
        std::size_t subset_size = 0;
        long double coordinate_scale = 0.0L;
        for(std::size_t index = 0; index < count; ++index){
            if((mask >> index) & 1U){
                indices[subset_size++] = index;
                coordinate_scale = std::max(
                    coordinate_scale, max_coordinate(simplex[index].point)
                );
            }
        }
        if(coordinate_scale == 0.0L) coordinate_scale = 1.0L;
        std::array<std::array<long double, 5>, 5> matrix{};
        std::array<long double, 5> right{};
        for(std::size_t row = 0; row < subset_size; ++row){
            const Point3 row_point =
                simplex[indices[row]].point / coordinate_scale;
            for(std::size_t column = 0; column < subset_size; ++column){
                matrix[row][column] = dot(
                    row_point,
                    simplex[indices[column]].point / coordinate_scale
                );
            }
            matrix[row][subset_size] = 1.0L;
            matrix[subset_size][row] = 1.0L;
        }
        right[subset_size] = 1.0L;
        std::array<long double, 5> solution{};
        if(!solve_linear_system(
            matrix, right, subset_size + 1, solution
        )) continue;
        bool feasible = true;
        for(std::size_t index = 0; index < subset_size; ++index){
            if(solution[index] < -tolerance){
                feasible = false;
                break;
            }
        }
        if(!feasible) continue;

        Point3 scaled_point{};
        for(std::size_t index = 0; index < subset_size; ++index){
            scaled_point += simplex[indices[index]].point
                / coordinate_scale * std::max(0.0L, solution[index]);
        }
        const long double squared = stable_scaled_squared(
            dot(scaled_point, scaled_point), coordinate_scale);
        if(squared >= best_squared) continue;
        best_squared = squared;
        best = {};
        long double weight_sum = 0.0L;
        for(std::size_t index = 0; index < subset_size; ++index){
            const long double weight = std::max(0.0L, solution[index]);
            if(weight <= tolerance && subset_size > 1) continue;
            best.simplex.push_back(simplex[indices[index]]);
            best.weights.push_back(weight);
            weight_sum += weight;
        }
        if(best.simplex.empty()){
            best.simplex.push_back(simplex[indices[0]]);
            best.weights.push_back(1.0L);
            weight_sum = 1.0L;
        }
        for(long double& weight: best.weights) weight /= weight_sum;
        for(std::size_t index = 0; index < best.simplex.size(); ++index){
            const long double weight = best.weights[index];
            best.point += best.simplex[index].point * weight;
            best.point_on_first +=
                best.simplex[index].point_on_first * weight;
            best.point_on_second +=
                best.simplex[index].point_on_second * weight;
        }
    }
    if(best.simplex.empty())[[unlikely]]{
        throw std::runtime_error("GJK could not reduce a degenerate simplex");
    }
    return best;
}

inline GJKSupportPoint3 support(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const Point3& direction
){
    const long double direction_scale = max_coordinate(direction);
    const Point3 scaled_direction = direction_scale == 0.0L
        ? Point3{1.0L, 0.0L, 0.0L} : direction / direction_scale;
    auto extreme = [&](const std::vector<Point3>& vertices, bool maximum){
        std::size_t answer = 0;
        long double coordinate_scale = 0.0L;
        for(const Point3& point: vertices){
            coordinate_scale = std::max(coordinate_scale, max_coordinate(point));
        }
        if(coordinate_scale == 0.0L) return vertices.front();
        long double best = dot(vertices.front() / coordinate_scale,
                               scaled_direction);
        for(std::size_t index = 1; index < vertices.size(); ++index){
            const long double value = dot(
                vertices[index] / coordinate_scale, scaled_direction
            );
            if((maximum && value > best) || (!maximum && value < best)){
                best = value;
                answer = index;
            }
        }
        return vertices[answer];
    };
    const Point3 point_on_first = extreme(first.vertices, true);
    const Point3 point_on_second = extreme(second.vertices, false);
    return {
        point_on_first - point_on_second, point_on_first, point_on_second
    };
}

inline void validate(const ConvexPolyhedron3& polyhedron){
    if(polyhedron.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("GJK requires nonempty convex polyhedra");
    }
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("GJK requires finite vertices");
        }
    }
}

inline long double scaled_difference(
    long double value,
    long double origin,
    long double scale
){
    if(value == 0.0L || origin == 0.0L ||
        std::signbit(value) == std::signbit(origin)){
        return (value - origin) / scale;
    }
    return value / scale - origin / scale;
}

inline Point3 scaled_difference(
    const Point3& point,
    const Point3& origin,
    long double scale
){
    return {
        scaled_difference(point.x, origin.x, scale),
        scaled_difference(point.y, origin.y, scale),
        scaled_difference(point.z, origin.z, scale)
    };
}

inline CollisionNormalization3 normalize_pair(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    validate(first);
    validate(second);
    CollisionNormalization3 normalization;
    normalization.origin = first.vertices.front();
    normalization.outer_scale = 0.0L;
    for(const Point3& point: first.vertices){
        normalization.outer_scale = std::max(
            normalization.outer_scale, max_coordinate(point)
        );
    }
    for(const Point3& point: second.vertices){
        normalization.outer_scale = std::max(
            normalization.outer_scale, max_coordinate(point)
        );
    }
    if(normalization.outer_scale == 0.0L){
        normalization.outer_scale = 1.0L;
    }
    normalization.inner_scale = 0.0L;
    auto measure = [&](const ConvexPolyhedron3& polyhedron){
        for(const Point3& point: polyhedron.vertices){
            normalization.inner_scale = std::max(
                normalization.inner_scale,
                max_coordinate(scaled_difference(
                    point, normalization.origin, normalization.outer_scale
                ))
            );
        }
    };
    measure(first);
    measure(second);
    if(normalization.inner_scale == 0.0L){
        normalization.inner_scale = 1.0L;
    }
    auto normalize = [&](const ConvexPolyhedron3& polyhedron){
        ConvexPolyhedron3 result = polyhedron;
        for(Point3& point: result.vertices){
            point = scaled_difference(
                point, normalization.origin, normalization.outer_scale
            ) / normalization.inner_scale;
        }
        return result;
    };
    normalization.first = normalize(first);
    normalization.second = normalize(second);
    return normalization;
}

inline long double restore_length(
    long double value,
    const CollisionNormalization3& normalization
){
    if(value == 0.0L) return 0.0L;
    const long double scaled = value * normalization.inner_scale;
    if(!std::isfinite(scaled))[[unlikely]]{
        throw std::overflow_error("3D collision length is not representable");
    }
    const long double result = scaled * normalization.outer_scale;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("3D collision length is not representable");
    }
    return result;
}

inline long double restore_coordinate(
    long double value,
    long double origin,
    const CollisionNormalization3& normalization
){
    const long double scaled = value * normalization.inner_scale;
    if(!std::isfinite(scaled))[[unlikely]]{
        throw std::overflow_error("3D collision point is not representable");
    }
    const long double result = std::fma(
        scaled, normalization.outer_scale, origin
    );
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("3D collision point is not representable");
    }
    return result;
}

inline Point3 restore_point(
    const Point3& point,
    const CollisionNormalization3& normalization
){
    return {
        restore_coordinate(point.x, normalization.origin.x, normalization),
        restore_coordinate(point.y, normalization.origin.y, normalization),
        restore_coordinate(point.z, normalization.origin.z, normalization)
    };
}

inline Point3 restore_vector(
    const Point3& point,
    const CollisionNormalization3& normalization
){
    return {
        restore_length(point.x, normalization),
        restore_length(point.y, normalization),
        restore_length(point.z, normalization)
    };
}

inline Point3 finite_unit_normal(Point3 normal){
    const long double scale = max_coordinate(normal);
    if(scale == 0.0L) return {};
    if(!std::isfinite(scale))[[unlikely]]{
        throw std::overflow_error("3D collision normal is not finite");
    }
    normal /= scale;
    const long double length = std::sqrt(dot(normal, normal));
    if(!(length > 0.0L) || !std::isfinite(length))[[unlikely]]{
        throw std::overflow_error("3D collision normal is not finite");
    }
    return normal / length;
}

inline bool same_support(
    const GJKSupportPoint3& first,
    const GJKSupportPoint3& second,
    long double tolerance
){
    const Point3 difference = first.point - second.point;
    const long double scale = std::max({
        1.0L, max_coordinate(first.point), max_coordinate(second.point)
    });
    return max_coordinate(difference) <= tolerance * scale;
}

inline GJKResult3 gjk_query_core(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance,
    std::size_t max_iterations
){
    validate(first);
    validate(second);
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("GJK tolerance must be finite and positive");
    }
    if(max_iterations == 0)[[unlikely]]{
        throw std::invalid_argument("GJK iteration limit must be positive");
    }

    Point3 direction = second.vertices.front() - first.vertices.front();
    if(max_coordinate(direction) == 0.0L) direction = {1.0L, 0.0L, 0.0L};
    std::vector<GJKSupportPoint3> simplex{support(first, second, direction)};
    ClosestSimplex3 closest = closest_simplex(simplex, tolerance);
    GJKResult3 result;

    for(std::size_t iteration = 0; iteration < max_iterations; ++iteration){
        result.iterations = iteration + 1;
        simplex = closest.simplex;
        const long double coordinate_scale = std::max(
            1.0L, max_coordinate(closest.point)
        );
        const long double squared = stable_squared_norm(closest.point);
        const long double distance = std::sqrt(std::max(0.0L, squared));
        if(distance <= tolerance * coordinate_scale){
            result.intersects = true;
            result.distance = 0.0L;
            result.point_on_first = closest.point_on_first;
            result.point_on_second = closest.point_on_second;
            result.simplex = simplex;
            result.converged = true;
            return result;
        }
        direction = -closest.point;
        const GJKSupportPoint3 next = support(first, second, direction);
        bool duplicate = false;
        for(const GJKSupportPoint3& point: simplex){
            duplicate = duplicate || same_support(point, next, tolerance);
        }
        const long double improvement = squared
            - stable_dot(closest.point, next.point);
        if(duplicate || improvement <= tolerance * std::max(1.0L, squared)){
            result.distance = distance;
            result.point_on_first = closest.point_on_first;
            result.point_on_second = closest.point_on_second;
            result.normal = finite_unit_normal(
                closest.point_on_second - closest.point_on_first
            );
            result.simplex = simplex;
            result.converged = true;
            return result;
        }
        simplex.push_back(next);
        closest = closest_simplex(simplex, tolerance);
    }

    const long double squared = stable_squared_norm(closest.point);
    const long double distance = std::sqrt(std::max(0.0L, squared));
    const long double coordinate_scale = std::max(
        1.0L, max_coordinate(closest.point)
    );
    result.intersects = distance <= tolerance * coordinate_scale;
    result.distance = result.intersects ? 0.0L : distance;
    result.point_on_first = closest.point_on_first;
    result.point_on_second = closest.point_on_second;
    if(result.distance > 0.0L){
        result.normal = finite_unit_normal(
            result.point_on_second - result.point_on_first
        );
    }
    result.simplex = closest.simplex;
    return result;
}

inline GJKResult3 restore_result(
    GJKResult3 result,
    const CollisionNormalization3& normalization
){
    result.distance = restore_length(result.distance, normalization);
    result.point_on_first = restore_point(
        result.point_on_first, normalization
    );
    result.point_on_second = restore_point(
        result.point_on_second, normalization
    );
    result.normal = finite_unit_normal(result.normal);
    for(GJKSupportPoint3& point: result.simplex){
        point.point = restore_vector(point.point, normalization);
        point.point_on_first = restore_point(
            point.point_on_first, normalization
        );
        point.point_on_second = restore_point(
            point.point_on_second, normalization
        );
    }
    return result;
}

inline std::vector<GJKSupportPoint3> normalize_simplex(
    const std::vector<GJKSupportPoint3>& simplex,
    const CollisionNormalization3& normalization
){
    std::vector<GJKSupportPoint3> result;
    result.reserve(simplex.size());
    for(const GJKSupportPoint3& point: simplex){
        if(!geometry3d_is_finite(point.point_on_first)
            || !geometry3d_is_finite(point.point_on_second))[[unlikely]]{
            throw std::invalid_argument(
                "GJK simplex requires finite support points"
            );
        }
        const Point3 first = scaled_difference(
            point.point_on_first, normalization.origin,
            normalization.outer_scale
        ) / normalization.inner_scale;
        const Point3 second = scaled_difference(
            point.point_on_second, normalization.origin,
            normalization.outer_scale
        ) / normalization.inner_scale;
        result.push_back({first - second, first, second});
    }
    return result;
}

}  // namespace gjk_3d_detail

inline GJKResult3 gjk_query_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-12L,
    std::size_t max_iterations = 96
){
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("GJK tolerance must be finite and positive");
    }
    if(max_iterations == 0)[[unlikely]]{
        throw std::invalid_argument("GJK iteration limit must be positive");
    }
    const gjk_3d_detail::CollisionNormalization3 normalization =
        gjk_3d_detail::normalize_pair(first, second);
    return gjk_3d_detail::restore_result(
        gjk_3d_detail::gjk_query_core(
            normalization.first, normalization.second,
            tolerance, max_iterations
        ),
        normalization
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_GJK_QUERY_3D_HPP_INCLUDED
