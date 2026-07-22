#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_LINE_PLANE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_LINE_PLANE_INTERSECTION_HPP_INCLUDED

#include <array>
#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "../scalar/dot.hpp"
#include "../predicate/is_finite.hpp"
#include "../predicate/on_plane.hpp"
#include "plane3_unit_normal.hpp"

namespace geometry3d_line_plane_detail{

struct IntersectionData{
    Point3 point{};
    int parameter_position = 0;
    int end_position = 0;
};

inline bool exact_same_point(const Point3& first, const Point3& second){
    return first.x == second.x && first.y == second.y
        && first.z == second.z;
}

inline std::array<geometry3d_adaptive_detail::ExactDyadic, 3> exact_direction(
    const Point3& first,
    const Point3& second
){
    const auto direction = geometry3d_plane_numeric_detail::exact_difference(
        second, first
    );
    if(direction[0].significand == 0
        && direction[1].significand == 0
        && direction[2].significand == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D linear primitive");
    }
    return direction;
}

inline std::optional<IntersectionData> intersection_data(
    const Point3& first,
    const Point3& second,
    const Plane3& plane
){
    using namespace geometry3d_adaptive_detail;
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second)
        || !geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument(
            "line-plane intersection requires finite inputs"
        );
    }
    if(plane.normal.x == 0.0L && plane.normal.y == 0.0L
        && plane.normal.z == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero 3D plane normal");
    }
    const auto direction = exact_direction(first, second);
    const Point3 normal = plane.normal;
    const auto denominator =
        geometry3d_plane_numeric_detail::exact_dot(normal, direction);
    const int denominator_sign =
        geometry3d_plane_numeric_detail::exact_dot_sign(denominator);
    if(denominator_sign == 0) return std::nullopt;

    const auto numerator =
        geometry3d_plane_numeric_detail::exact_dot_difference(
            normal, plane.point, first
        );
    const auto end_numerator =
        geometry3d_plane_numeric_detail::exact_dot_difference(
            normal, plane.point, second
        );
    const int numerator_sign =
        geometry3d_plane_numeric_detail::exact_dot_sign(numerator);
    const int end_numerator_sign =
        geometry3d_plane_numeric_detail::exact_dot_sign(end_numerator);
    const int parameter_position = numerator_sign == 0
        ? 0 : numerator_sign * denominator_sign;
    const int end_position = end_numerator_sign == 0
        ? 0 : end_numerator_sign * denominator_sign;

    const auto coordinate = [&](std::size_t index, long double origin){
        const ExactDyadic coordinate_numerator = add(
            multiply(exact_dyadic(origin), denominator.value),
            multiply(direction[index], numerator.value)
        );
        return geometry3d_plane_numeric_detail::exact_ratio(
            coordinate_numerator,
            denominator.value,
            "line-plane intersection point is not representable"
        );
    };
    const Point3 point{
        coordinate(0, first.x),
        coordinate(1, first.y),
        coordinate(2, first.z),
    };
    return IntersectionData{point, parameter_position, end_position};
}

inline int parameter_sign(const IntersectionData& data){
    return data.parameter_position;
}

inline int parameter_end_sign(const IntersectionData& data){
    return data.end_position;
}

}  // namespace geometry3d_line_plane_detail

inline Point3 line_plane_intersection(const Line3& line, const Plane3& plane){
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        line.a, line.b, plane
    );
    if(!intersection)[[unlikely]]{
        throw std::domain_error(
            "3D line and plane do not have a unique intersection"
        );
    }
    return intersection->point;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_LINE_PLANE_INTERSECTION_HPP_INCLUDED
