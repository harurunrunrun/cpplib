#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"
#include "unit.hpp"

namespace geometry3d_plane_plane_detail{

inline Point3 exact_cross_direction(
    const Point3& first,
    const Point3& second
){
    using namespace geometry3d_adaptive_detail;
    const std::array<ExactDyadic, 3> left{
        exact_dyadic(first.x), exact_dyadic(first.y), exact_dyadic(first.z)
    };
    const std::array<ExactDyadic, 3> right{
        exact_dyadic(second.x), exact_dyadic(second.y), exact_dyadic(second.z)
    };
    const std::array<ExactDyadic, 3> product{
        subtract(multiply(left[1], right[2]), multiply(left[2], right[1])),
        subtract(multiply(left[2], right[0]), multiply(left[0], right[2])),
        subtract(multiply(left[0], right[1]), multiply(left[1], right[0])),
    };
    std::array<geometry3d_plane_numeric_detail::NormalizedDyadic, 3>
        normalized{};
    int maximum_exponent = std::numeric_limits<int>::min();
    for(std::size_t index = 0; index < 3; ++index){
        if(sign(product[index]) == 0) continue;
        normalized[index] =
            geometry3d_plane_numeric_detail::normalize_exact_dyadic(
                product[index]
            );
        maximum_exponent = std::max(
            maximum_exponent, normalized[index].exponent
        );
    }
    if(maximum_exponent == std::numeric_limits<int>::min())[[unlikely]]{
        throw std::domain_error(
            "3D planes do not have a unique intersection line"
        );
    }
    const auto component = [&](std::size_t index){
        if(sign(product[index]) == 0) return 0.0L;
        return std::scalbn(
            normalized[index].mantissa,
            normalized[index].exponent - maximum_exponent
        );
    };
    return {component(0), component(1), component(2)};
}

inline std::optional<Point3> displaced_point(
    const Point3& point,
    const Point3& unit_direction,
    int sign
){
    const long double components[3]{
        point.x, point.y, point.z
    };
    const long double directions[3]{
        unit_direction.x, unit_direction.y, unit_direction.z
    };
    std::size_t dominant = 0;
    for(std::size_t index = 1; index < 3; ++index){
        if(std::abs(directions[index]) > std::abs(directions[dominant])){
            dominant = index;
        }
    }
    const long double signed_direction =
        static_cast<long double>(sign) * directions[dominant];
    const long double target = std::copysign(
        std::numeric_limits<long double>::infinity(), signed_direction
    );
    const long double next = std::nextafter(components[dominant], target);
    if(!std::isfinite(next)) return std::nullopt;
    const long double coordinate_step = std::max(
        1.0L, 2.0L * std::abs(next - components[dominant])
    );
    const long double step = static_cast<long double>(sign)
        * coordinate_step / std::abs(directions[dominant]);
    if(!std::isfinite(step)) return std::nullopt;
    const Point3 result{
        std::fma(unit_direction.x, step, point.x),
        std::fma(unit_direction.y, step, point.y),
        std::fma(unit_direction.z, step, point.z),
    };
    if(!geometry3d_is_finite(result)
        || (result.x == point.x && result.y == point.y
            && result.z == point.z)){
        return std::nullopt;
    }
    return result;
}

inline Point3 second_line_point(
    const Point3& point,
    const Point3& unit_direction
){
    if(const auto forward = displaced_point(point, unit_direction, 1)){
        return *forward;
    }
    if(const auto backward = displaced_point(point, unit_direction, -1)){
        return *backward;
    }
    throw std::overflow_error(
        "plane-plane intersection line is not representable"
    );
}

}  // namespace geometry3d_plane_plane_detail

inline Line3 plane_plane_intersection(
    const Plane3& first,
    const Plane3& second
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second))
        [[unlikely]]{
        throw std::invalid_argument(
            "plane-plane intersection requires finite planes"
        );
    }
    static_cast<void>(plane3_unit_normal(first));
    static_cast<void>(plane3_unit_normal(second));
    const Point3 direction =
        geometry3d_plane_plane_detail::exact_cross_direction(
            first.normal, second.normal
        );
    using namespace geometry3d_adaptive_detail;
    const std::array<ExactDyadic, 3> first_coefficients{
        exact_dyadic(first.normal.x),
        exact_dyadic(first.normal.y),
        exact_dyadic(first.normal.z),
    };
    const std::array<ExactDyadic, 3> second_coefficients{
        exact_dyadic(second.normal.x),
        exact_dyadic(second.normal.y),
        exact_dyadic(second.normal.z),
    };

    const Point3 unit_direction = unit(direction);
    const std::array<long double, 3> direction_values{
        unit_direction.x, unit_direction.y, unit_direction.z
    };
    std::size_t dominant = 0;
    for(std::size_t index = 1; index < 3; ++index){
        if(std::abs(direction_values[index])
            > std::abs(direction_values[dominant])){
            dominant = index;
        }
    }
    const std::size_t first_coordinate = (dominant + 1) % 3;
    const std::size_t second_coordinate = (dominant + 2) % 3;
    const ExactDyadic determinant = subtract(
        multiply(
            first_coefficients[first_coordinate],
            second_coefficients[second_coordinate]
        ),
        multiply(
            first_coefficients[second_coordinate],
            second_coefficients[first_coordinate]
        )
    );
    if(sign(determinant) == 0)[[unlikely]]{
        throw std::logic_error("plane-plane exact determinant vanished");
    }
    const ExactDyadic first_offset =
        geometry3d_plane_numeric_detail::exact_dot_difference(
            first.normal, first.point, Point3{}
        ).value;
    const ExactDyadic second_offset =
        geometry3d_plane_numeric_detail::exact_dot_difference(
            second.normal, second.point, Point3{}
        ).value;
    const auto point_at = [&](long double fixed_coordinate){
        const ExactDyadic fixed = exact_dyadic(fixed_coordinate);
        const ExactDyadic first_right = subtract(
            first_offset, multiply(first_coefficients[dominant], fixed)
        );
        const ExactDyadic second_right = subtract(
            second_offset, multiply(second_coefficients[dominant], fixed)
        );
        const ExactDyadic first_numerator = subtract(
            multiply(
                first_right, second_coefficients[second_coordinate]
            ),
            multiply(
                first_coefficients[second_coordinate], second_right
            )
        );
        const ExactDyadic second_numerator = subtract(
            multiply(first_coefficients[first_coordinate], second_right),
            multiply(first_right, second_coefficients[first_coordinate])
        );
        std::array<long double, 3> coordinates{};
        coordinates[dominant] = fixed_coordinate;
        coordinates[first_coordinate] =
            geometry3d_plane_numeric_detail::exact_ratio(
                first_numerator,
                determinant,
                "plane-plane intersection point is not representable"
            );
        coordinates[second_coordinate] =
            geometry3d_plane_numeric_detail::exact_ratio(
                second_numerator,
                determinant,
                "plane-plane intersection point is not representable"
            );
        return Point3{
            coordinates[0], coordinates[1], coordinates[2]
        };
    };
    const std::array<long double, 3> first_point_coordinates{
        first.point.x, first.point.y, first.point.z
    };
    const Point3 point = point_at(first_point_coordinates[dominant]);
    const long double maximum =
        std::numeric_limits<long double>::max() / 4.0L;
    const std::array<long double, 3> point_coordinates{
        point.x, point.y, point.z
    };
    const std::array<long double, 7> candidates{
        0.0L, 1.0L, -1.0L, maximum, -maximum,
        std::nextafter(point_coordinates[dominant],
            std::numeric_limits<long double>::infinity()),
        std::nextafter(point_coordinates[dominant],
            -std::numeric_limits<long double>::infinity()),
    };
    for(const long double candidate: candidates){
        if(!std::isfinite(candidate)) continue;
        try{
            const Point3 endpoint = point_at(candidate);
            if(endpoint != point && on_plane(first, endpoint)
                && on_plane(second, endpoint)){
                return {point, endpoint};
            }
        }catch(const std::overflow_error&){
        }
    }
    throw std::overflow_error(
        "plane-plane intersection line is not representable"
    );
}
