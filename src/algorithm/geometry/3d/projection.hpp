#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"
#include "unit.hpp"

namespace geometry3d_linear_closest_detail{

enum class Kind{
    line,
    ray,
    segment,
};

struct Input{
    Point3 first{};
    Point3 second{};
    Kind kind = Kind::segment;
};

struct Prepared{
    Point3 origin{};
    Point3 direction{1, 0, 0};
    long double lower = 0.0L;
    long double upper = 0.0L;
};

inline bool nonzero(const Geometry3DNormalizedDifference& difference){
    return difference.value.x != 0.0L
        || difference.value.y != 0.0L
        || difference.value.z != 0.0L;
}

inline long double checked_scale_product(
    long double normalized,
    long double scale,
    const char* message
){
    if(normalized == 0.0L) return 0.0L;
    if(std::abs(normalized)
        > std::numeric_limits<long double>::max() / scale){
        throw std::overflow_error(message);
    }
    const long double result = normalized * scale;
    if(!std::isfinite(result) || result == 0.0L)[[unlikely]]{
        throw std::overflow_error(message);
    }
    return result;
}

inline long double checked_distance(
    const Point3& first,
    const Point3& second,
    const char* message = "3D distance is not representable"
){
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(first, second);
    return checked_scale_product(
        std::hypot(
            difference.value.x,
            difference.value.y,
            difference.value.z
        ),
        difference.scale,
        message
    );
}

inline Point3 restore(
    const Point3& local,
    const Point3& anchor,
    long double scale
){
    const Point3 result{
        std::fma(local.x, scale, anchor.x),
        std::fma(local.y, scale, anchor.y),
        std::fma(local.z, scale, anchor.z),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "3D closest point is not representable"
        );
    }
    return result;
}

inline bool exact_orthogonal_differences(
    const Point3& first_end,
    const Point3& first_begin,
    const Point3& second_end,
    const Point3& second_begin
){
    using namespace geometry3d_adaptive_detail;
    const auto first = geometry3d_plane_numeric_detail::exact_difference(
        first_end, first_begin
    );
    const auto second = geometry3d_plane_numeric_detail::exact_difference(
        second_end, second_begin
    );
    ExactDyadic result{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result = add(
            result,
            multiply(first[coordinate], second[coordinate])
        );
    }
    return sign(result) == 0;
}

inline std::pair<Point3, Point3> query(
    const Input& first_input,
    const Input& second_input
){
    for(const Point3& point: {
        first_input.first, first_input.second,
        second_input.first, second_input.second
    }){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "3D closest query requires finite points"
            );
        }
    }
    const Geometry3DNormalizedDifference first_direction =
        geometry3d_normalized_difference(
            first_input.second, first_input.first
        );
    const Geometry3DNormalizedDifference second_direction =
        geometry3d_normalized_difference(
            second_input.second, second_input.first
        );
    if(!nonzero(first_direction)
        && first_input.kind != Kind::segment)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    if(!nonzero(second_direction)
        && second_input.kind != Kind::segment)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    const Geometry3DNormalizedDifference origin_difference =
        geometry3d_normalized_difference(
            second_input.first, first_input.first
        );
    long double scale = 0.0L;
    if(nonzero(origin_difference)) scale = origin_difference.scale;
    if(first_input.kind == Kind::segment && nonzero(first_direction)){
        scale = std::max(scale, first_direction.scale);
    }
    if(second_input.kind == Kind::segment && nonzero(second_direction)){
        scale = std::max(scale, second_direction.scale);
    }
    if(scale == 0.0L) scale = 1.0L;

    const auto prepare = [&](const Input& input,
                             const Geometry3DNormalizedDifference& direction){
        Prepared result;
        result.origin = geometry3d_safe_normalized_difference(
            input.first, first_input.first, scale
        );
        if(nonzero(direction)){
            result.direction = unit(direction.value);
        }
        if(input.kind == Kind::line){
            result.lower = -std::numeric_limits<long double>::infinity();
            result.upper = std::numeric_limits<long double>::infinity();
        }else if(input.kind == Kind::ray){
            result.lower = 0.0L;
            result.upper = std::numeric_limits<long double>::infinity();
        }else if(nonzero(direction)){
            const long double ratio = direction.scale / scale;
            if(ratio == 0.0L)[[unlikely]]{
                throw std::overflow_error(
                    "3D segment length is not representable locally"
                );
            }
            result.upper = std::hypot(
                direction.value.x,
                direction.value.y,
                direction.value.z
            ) * ratio;
            if(result.upper == 0.0L)[[unlikely]]{
                throw std::overflow_error(
                    "3D segment length is not representable locally"
                );
            }
        }
        return result;
    };
    const Prepared first = prepare(first_input, first_direction);
    const Prepared second = prepare(second_input, second_direction);

    const auto clamp = [](long double value, const Prepared& primitive){
        return std::clamp(value, primitive.lower, primitive.upper);
    };
    const auto inside = [](long double value, const Prepared& primitive){
        return std::isfinite(value)
            && value >= primitive.lower && value <= primitive.upper;
    };
    const Point3 offset = first.origin - second.origin;
    const long double coupling = dot(first.direction, second.direction);
    const long double first_offset = dot(first.direction, offset);
    const long double second_offset = dot(second.direction, offset);
    long double best_distance =
        std::numeric_limits<long double>::infinity();
    Point3 best_first{};
    Point3 best_second{};
    bool found = false;
    bool best_points_representable = true;
    const auto consider = [&](long double first_parameter,
                              long double second_parameter){
        if(!inside(first_parameter, first)
            || !inside(second_parameter, second)) return;
        const Point3 on_first{
            std::fma(first.direction.x, first_parameter, first.origin.x),
            std::fma(first.direction.y, first_parameter, first.origin.y),
            std::fma(first.direction.z, first_parameter, first.origin.z),
        };
        const Point3 on_second{
            std::fma(second.direction.x, second_parameter, second.origin.x),
            std::fma(second.direction.y, second_parameter, second.origin.y),
            std::fma(second.direction.z, second_parameter, second.origin.z),
        };
        if(!geometry3d_is_finite(on_first)
            || !geometry3d_is_finite(on_second)) return;
        const long double current = std::hypot(
            on_first.x - on_second.x,
            on_first.y - on_second.y,
            on_first.z - on_second.z
        );
        if(!std::isfinite(current)) return;
        if(!found || current < best_distance){
            found = true;
            best_distance = current;
            best_first = on_first;
            best_second = on_second;
            best_points_representable = true;
        }
    };

    const Point3 cross_direction =
        cross(first.direction, second.direction);
    const long double sine = std::hypot(
        cross_direction.x, cross_direction.y, cross_direction.z
    );
    if(sine != 0.0L){
        const Point3 perpendicular =
            cross(cross_direction / sine, first.direction);
        const long double second_parameter =
            dot(perpendicular, offset) / sine;
        const long double first_parameter =
            coupling * second_parameter - first_offset;
        const auto extended_inside = [](long double value,
                                        const Prepared& primitive){
            return !std::isnan(value)
                && value >= primitive.lower && value <= primitive.upper;
        };
        if(std::isfinite(first_parameter)
            && std::isfinite(second_parameter)){
            consider(first_parameter, second_parameter);
        }else if(extended_inside(first_parameter, first)
                 && extended_inside(second_parameter, second)){
            const long double current = std::abs(dot(
                offset, cross_direction / sine
            ));
            if(!found || current < best_distance){
                found = true;
                best_distance = current;
                best_points_representable = false;
            }
        }
    }

    const auto consider_first_boundary = [&](long double parameter){
        if(std::isfinite(parameter)){
            consider(
                parameter,
                clamp(
                    second_offset + coupling * parameter,
                    second
                )
            );
        }
    };
    const auto consider_second_boundary = [&](long double parameter){
        if(std::isfinite(parameter)){
            consider(
                clamp(
                    -first_offset + coupling * parameter,
                    first
                ),
                parameter
            );
        }
    };
    consider_first_boundary(first.lower);
    if(first.upper != first.lower){
        consider_first_boundary(first.upper);
    }
    consider_second_boundary(second.lower);
    if(second.upper != second.lower){
        consider_second_boundary(second.upper);
    }
    const long double canonical_first = clamp(0.0L, first);
    consider(
        canonical_first,
        clamp(
            second_offset + coupling * canonical_first,
            second
        )
    );
    const long double canonical_second = clamp(0.0L, second);
    consider(
        clamp(
            -first_offset + coupling * canonical_second,
            first
        ),
        canonical_second
    );
    if(!found)[[unlikely]]{
        throw std::overflow_error(
            "3D closest points are not representable"
        );
    }
    if(!best_points_representable)[[unlikely]]{
        throw std::overflow_error(
            "3D closest points are not representable"
        );
    }
    return {
        restore(best_first, first_input.first, scale),
        restore(best_second, first_input.first, scale),
    };
}

}  // namespace geometry3d_linear_closest_detail

inline Point3 projection(const Line3& line, const Point3& point){
    using namespace geometry3d_linear_closest_detail;
    const Geometry3DNormalizedDifference direction =
        geometry3d_normalized_difference(line.b, line.a);
    if(!nonzero(direction))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(point, line.a);
    const Point3 direction_unit = unit(direction.value);
    const long double parameter = dot(difference.value, direction_unit);
    const long double difference_length = std::hypot(
        difference.value.x,
        difference.value.y,
        difference.value.z
    );
    if(parameter == 0.0L
        || (std::abs(parameter) <= GEOMETRY3D_EPS * difference_length
            && exact_orthogonal_differences(
                line.b, line.a, point, line.a
            ))){
        return line.a;
    }
    return restore(
        direction_unit * parameter,
        line.a,
        difference.scale
    );
}

inline Point3 projection(const Plane3& plane, const Point3& point){
    const Point3 normal = plane3_unit_normal(plane);
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(point, plane.point);
    const long double parameter = dot(difference.value, normal);
    if(parameter == 0.0L
        || (std::abs(parameter) <= GEOMETRY3D_EPS
                * std::hypot(
                    difference.value.x,
                    difference.value.y,
                    difference.value.z
                )
            && geometry3d_adaptive_detail::sign(
                geometry3d_plane_numeric_detail::exact_dot_difference(
                    plane.normal, point, plane.point
                ).value
            ) == 0)){
        return point;
    }
    const Point3 local = difference.value - normal * parameter;
    return geometry3d_linear_closest_detail::restore(
        local, plane.point, difference.scale
    );
}
