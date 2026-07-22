#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CIRCUMCIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CIRCUMCIRCLE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "../scalar/abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../point/cross.hpp"
#include "../predicate/is_finite.hpp"
#include "../predicate/on_plane.hpp"
#include "../point/unit.hpp"

namespace geometry3d_circumcircle_detail{

using ExactVector = std::array<
    geometry3d_adaptive_detail::ExactDyadic, 3
>;

inline geometry3d_adaptive_detail::ExactDyadic exact_dot(
    const ExactVector& first,
    const ExactVector& second
){
    using namespace geometry3d_adaptive_detail;
    ExactDyadic result{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result = add(result, multiply(
            first[coordinate], second[coordinate]
        ));
    }
    return result;
}

inline ExactVector exact_cross(
    const ExactVector& first,
    const ExactVector& second
){
    using namespace geometry3d_adaptive_detail;
    return {
        subtract(multiply(first[1], second[2]),
                 multiply(first[2], second[1])),
        subtract(multiply(first[2], second[0]),
                 multiply(first[0], second[2])),
        subtract(multiply(first[0], second[1]),
                 multiply(first[1], second[0])),
    };
}

inline bool exact_zero(const ExactVector& vector){
    using geometry3d_adaptive_detail::sign;
    return sign(vector[0]) == 0 && sign(vector[1]) == 0
        && sign(vector[2]) == 0;
}

inline Point3 normalized_exact_vector(const ExactVector& vector){
    using namespace geometry3d_adaptive_detail;
    std::array<geometry3d_plane_numeric_detail::NormalizedDyadic, 3>
        normalized{};
    int maximum_exponent = std::numeric_limits<int>::min();
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        if(sign(vector[coordinate]) == 0) continue;
        normalized[coordinate] =
            geometry3d_plane_numeric_detail::normalize_exact_dyadic(
                vector[coordinate]
            );
        maximum_exponent = std::max(
            maximum_exponent, normalized[coordinate].exponent
        );
    }
    if(maximum_exponent == std::numeric_limits<int>::min()) return {};
    const auto value = [&](std::size_t coordinate){
        if(sign(vector[coordinate]) == 0) return 0.0L;
        return std::scalbn(
            normalized[coordinate].mantissa,
            normalized[coordinate].exponent - maximum_exponent
        );
    };
    return {value(0), value(1), value(2)};
}

inline long double checked_distance(
    const Point3& first,
    const Point3& second,
    const char* message
){
    const Geometry3DNormalizedDifference difference =
        geometry3d_normalized_difference(first, second);
    const long double normalized = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    if(normalized >
        std::numeric_limits<long double>::max() / difference.scale){
        throw std::overflow_error(message);
    }
    const long double result = normalized * difference.scale;
    if(!std::isfinite(result)
        || (normalized != 0.0L && result == 0.0L))[[unlikely]]{
        throw std::overflow_error(message);
    }
    return result;
}

inline Point3 exact_affine_point(
    const Point3& anchor,
    const ExactVector& numerator,
    const geometry3d_adaptive_detail::ExactDyadic& denominator,
    const char* message
){
    using namespace geometry3d_adaptive_detail;
    const std::array<long double, 3> anchor_coordinates{
        anchor.x, anchor.y, anchor.z
    };
    std::array<long double, 3> result{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        result[coordinate] =
            geometry3d_plane_numeric_detail::exact_ratio(
                add(
                    multiply(
                        exact_dyadic(anchor_coordinates[coordinate]),
                        denominator
                    ),
                    numerator[coordinate]
                ),
                denominator,
                message
            );
    }
    return {result[0], result[1], result[2]};
}

}  // namespace geometry3d_circumcircle_detail

inline Circle3 circumcircle(const Triangle3& triangle){
    using namespace geometry3d_adaptive_detail;
    using namespace geometry3d_circumcircle_detail;
    if(!geometry3d_is_finite(triangle))[[unlikely]]{
        throw std::invalid_argument("circumcircle requires finite points");
    }
    const ExactVector first =
        geometry3d_plane_numeric_detail::exact_difference(
            triangle.b, triangle.a
        );
    const ExactVector second =
        geometry3d_plane_numeric_detail::exact_difference(
            triangle.c, triangle.a
        );
    const ExactVector normal = exact_cross(first, second);
    if(exact_zero(normal))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const ExactDyadic first_squared = exact_dot(first, first);
    const ExactDyadic second_squared = exact_dot(second, second);
    const ExactVector first_cofactor = exact_cross(second, normal);
    const ExactVector second_cofactor = exact_cross(normal, first);
    ExactVector numerator{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        numerator[coordinate] = add(
            multiply(first_squared, first_cofactor[coordinate]),
            multiply(second_squared, second_cofactor[coordinate])
        );
    }
    const ExactDyadic denominator = multiply(
        exact_dyadic(2.0L), exact_dot(normal, normal)
    );
    const Point3 center = exact_affine_point(
        triangle.a, numerator, denominator,
        "3D circumcircle center is not representable"
    );
    return {
        center,
        unit(normalized_exact_vector(normal)),
        checked_distance(
            center, triangle.a,
            "3D circumcircle radius is not representable"
        ),
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SHAPE_CIRCUMCIRCLE_HPP_INCLUDED
