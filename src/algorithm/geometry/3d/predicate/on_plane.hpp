#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_PLANE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <utility>


#include "adaptive_orient3d.hpp"
#include "../core/geometry_primitives.hpp"
#include "../scalar/dot.hpp"
#include "is_finite.hpp"
#include "../point/plane3_unit_normal.hpp"

namespace geometry3d_plane_numeric_detail{

struct ScaledDifference{
    Point3 normalized{};
    long double scale = 1.0L;
};

inline long double coordinate_scale(std::initializer_list<Point3> points){
    long double scale = 1.0L;
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "plane computation requires finite point coordinates"
            );
        }
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    return scale;
}

inline ScaledDifference scaled_difference(
    const Point3& first,
    const Point3& second
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second))
        [[unlikely]]{
        throw std::invalid_argument(
            "scaled difference requires finite point coordinates"
        );
    }
    const Point3 raw{
        first.x - second.x,
        first.y - second.y,
        first.z - second.z,
    };
    if(geometry3d_is_finite(raw)){
        const long double scale = std::max({
            std::abs(raw.x), std::abs(raw.y), std::abs(raw.z)
        });
        if(scale == 0.0L) return {};
        return {raw / scale, scale};
    }

    const long double scale = coordinate_scale({first, second});
    return {
        geometry3d_safe_normalized_difference(first, second, scale),
        scale,
    };
}

inline long double normalized_maximum(const Point3& point){
    return std::max({
        std::abs(point.x), std::abs(point.y), std::abs(point.z)
    });
}

inline int difference_scaled_sign(
    long double normalized_value,
    const ScaledDifference& difference
){
    const long double normalized_scale = normalized_maximum(
        difference.normalized
    );
    return geometry3d_scaled_sign(normalized_value, normalized_scale);
}

inline Point3 restore_scaled_point(
    const Point3& normalized,
    long double scale,
    const char* message
){
    const long double maximum = std::numeric_limits<long double>::max();
    for(const long double coordinate: {
        normalized.x, normalized.y, normalized.z
    }){
        if(std::abs(coordinate) > maximum / scale)[[unlikely]]{
            throw std::overflow_error(message);
        }
    }
    const Point3 result = normalized * scale;
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(message);
    }
    return result;
}
struct ExactDot{
    geometry3d_adaptive_detail::ExactDyadic value{};
    geometry3d_adaptive_detail::ExactDyadic magnitude{};
};

inline geometry3d_adaptive_detail::ExactDyadic exact_absolute(
    geometry3d_adaptive_detail::ExactDyadic value
){
    if(value.significand < 0) value.significand = -value.significand;
    return value;
}

inline std::array<geometry3d_adaptive_detail::ExactDyadic, 3>
exact_difference(const Point3& first, const Point3& second){
    using namespace geometry3d_adaptive_detail;
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second))
        [[unlikely]]{
        throw std::invalid_argument(
            "exact difference requires finite point coordinates"
        );
    }
    return {
        subtract(exact_dyadic(first.x), exact_dyadic(second.x)),
        subtract(exact_dyadic(first.y), exact_dyadic(second.y)),
        subtract(exact_dyadic(first.z), exact_dyadic(second.z)),
    };
}

inline ExactDot exact_dot(
    const Point3& coefficients,
    const std::array<geometry3d_adaptive_detail::ExactDyadic, 3>& values
){
    using namespace geometry3d_adaptive_detail;
    const std::array<ExactDyadic, 3> coefficient_values{
        exact_dyadic(coefficients.x),
        exact_dyadic(coefficients.y),
        exact_dyadic(coefficients.z),
    };
    ExactDot result;
    for(std::size_t index = 0; index < 3; ++index){
        const ExactDyadic term = multiply(
            coefficient_values[index], values[index]
        );
        result.value = add(result.value, term);
        result.magnitude = add(result.magnitude, exact_absolute(term));
    }
    return result;
}

inline ExactDot exact_dot_difference(
    const Point3& coefficients,
    const Point3& first,
    const Point3& second
){
    return exact_dot(coefficients, exact_difference(first, second));
}

inline int exact_dot_sign(
    const ExactDot& product,
    long double tolerance_multiplier = 1.0L
){
    using namespace geometry3d_adaptive_detail;
    if(!std::isfinite(tolerance_multiplier)
        || tolerance_multiplier < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "exact dot sign requires a finite nonnegative tolerance"
        );
    }
    const int value_sign = sign(product.value);
    if(value_sign == 0) return 0;
    const ExactDyadic tolerance = multiply(
        exact_dyadic(GEOMETRY3D_EPS * tolerance_multiplier),
        product.magnitude
    );
    return sign(subtract(exact_absolute(product.value), tolerance)) > 0
        ? value_sign : 0;
}

struct NormalizedDyadic{
    long double mantissa = 0.0L;
    int exponent = 0;
};

inline long double exact_integer_leading_value(ExactInteger value){
    constexpr std::uint64_t base = std::uint64_t{1} << 32;
    long double result = 0.0L;
    long double place = 1.0L;
    while(!value.is_zero()){
        auto [quotient, remainder] = value.divmod(base);
        result += place * static_cast<long double>(remainder);
        value = std::move(quotient);
        if(!value.is_zero()) place = std::ldexp(place, 32);
    }
    return result;
}

inline NormalizedDyadic normalize_exact_dyadic(
    const geometry3d_adaptive_detail::ExactDyadic& value
){
    if(value.significand == 0) return {};
    ExactInteger magnitude = value.significand;
    const bool negative = magnitude < 0;
    if(negative) magnitude = -magnitude;
    const std::size_t bit_count = magnitude.bit_length();
    constexpr std::size_t digits =
        static_cast<std::size_t>(std::numeric_limits<long double>::digits);
    const std::size_t discarded = bit_count > digits
        ? bit_count - digits : 0;
    const ExactInteger leading = magnitude >> discarded;
    const long double leading_value = exact_integer_leading_value(leading);
    int leading_exponent = 0;
    long double mantissa = std::frexp(leading_value, &leading_exponent);
    if(negative) mantissa = -mantissa;
    if(discarded > static_cast<std::size_t>(
        (std::numeric_limits<long long>::max)()
    ))[[unlikely]]{
        throw std::overflow_error("exact dyadic exponent is not representable");
    }
    const long long exponent =
        static_cast<long long>(value.exponent)
        + static_cast<long long>(discarded)
        + static_cast<long long>(leading_exponent);
    if(exponent < std::numeric_limits<int>::min()
        || exponent > std::numeric_limits<int>::max())[[unlikely]]{
        throw std::overflow_error("exact dyadic exponent is not representable");
    }
    return {mantissa, static_cast<int>(exponent)};
}

struct PositiveDivision{
    ExactInteger quotient;
    ExactInteger remainder;
};

inline PositiveDivision positive_divmod(
    ExactInteger numerator,
    const ExactInteger& positive_denominator
){
    if(numerator < 0 || positive_denominator <= 0)[[unlikely]]{
        throw std::domain_error(
            "exact ratio division requires nonnegative numerator and "
            "positive denominator"
        );
    }
    if(numerator < positive_denominator){
        return {0, std::move(numerator)};
    }
    const std::size_t shift =
        numerator.bit_length() - positive_denominator.bit_length();
    ExactInteger shifted_denominator = positive_denominator << shift;
    ExactInteger bit = ExactInteger(1) << shift;
    ExactInteger quotient = 0;
    for(std::size_t remaining_shift = shift;; --remaining_shift){
        if(numerator >= shifted_denominator){
            numerator -= shifted_denominator;
            quotient += bit;
        }
        if(remaining_shift == 0) break;
        shifted_denominator >>= 1;
        bit >>= 1;
    }
    return {std::move(quotient), std::move(numerator)};
}

inline long double exact_ratio(
    const geometry3d_adaptive_detail::ExactDyadic& numerator,
    const geometry3d_adaptive_detail::ExactDyadic& denominator,
    const char* overflow_message
){
    if(denominator.significand == 0)[[unlikely]]{
        throw std::domain_error("exact ratio has zero denominator");
    }
    if(numerator.significand == 0) return 0.0L;

    ExactInteger numerator_magnitude = numerator.significand;
    ExactInteger denominator_magnitude = denominator.significand;
    const bool negative = (numerator_magnitude < 0)
        != (denominator_magnitude < 0);
    if(numerator_magnitude < 0) numerator_magnitude = -numerator_magnitude;
    if(denominator_magnitude < 0){
        denominator_magnitude = -denominator_magnitude;
    }

    const std::size_t numerator_bits = numerator_magnitude.bit_length();
    const std::size_t denominator_bits = denominator_magnitude.bit_length();
    if(numerator_bits > static_cast<std::size_t>(
            (std::numeric_limits<long long>::max)())
        || denominator_bits > static_cast<std::size_t>(
            (std::numeric_limits<long long>::max)()))[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }

    long long ratio_exponent = static_cast<long long>(numerator_bits)
        - static_cast<long long>(denominator_bits);
    if(ratio_exponent >= 0){
        if(numerator_magnitude
            < (denominator_magnitude
                << static_cast<std::size_t>(ratio_exponent))){
            --ratio_exponent;
        }
    }else if((numerator_magnitude
        << static_cast<std::size_t>(-ratio_exponent))
        < denominator_magnitude){
        --ratio_exponent;
    }

    constexpr long long precision =
        std::numeric_limits<long double>::digits;
    const long long quotient_shift = precision - 1 - ratio_exponent;
    if(quotient_shift >= 0){
        numerator_magnitude <<= static_cast<std::size_t>(quotient_shift);
    }else{
        denominator_magnitude <<=
            static_cast<std::size_t>(-quotient_shift);
    }
    PositiveDivision division = positive_divmod(
        std::move(numerator_magnitude), denominator_magnitude
    );
    const ExactInteger twice_remainder = division.remainder << 1;
    const bool round_up = twice_remainder > denominator_magnitude
        || (twice_remainder == denominator_magnitude
            && division.quotient.divmod(2).second != 0);
    if(round_up) division.quotient += 1;

    long double quotient = exact_integer_leading_value(division.quotient);
    if(negative) quotient = -quotient;
    const long long exponent =
        static_cast<long long>(numerator.exponent)
        - static_cast<long long>(denominator.exponent)
        + ratio_exponent - (precision - 1);
    if(exponent < std::numeric_limits<int>::min()
        || exponent > std::numeric_limits<int>::max())[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    const long double result = std::scalbn(
        quotient,
        static_cast<int>(exponent)
    );
    if(!std::isfinite(result) || result == 0.0L)[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    return result;
}

inline long double add_finite(
    long double first,
    long double second,
    const char* overflow_message
){
    const long double result = first + second;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    return result;
}


}  // namespace geometry3d_plane_numeric_detail

inline bool on_plane(const Plane3& plane, const Point3& point){
    if(!geometry3d_is_finite(plane) || !geometry3d_is_finite(point))
        [[unlikely]]{
        throw std::invalid_argument("on_plane requires finite inputs");
    }
    if(plane.normal.x == 0.0L && plane.normal.y == 0.0L
        && plane.normal.z == 0.0L)[[unlikely]]{
        throw std::invalid_argument("zero 3D plane normal");
    }
    const auto product = geometry3d_plane_numeric_detail::exact_dot_difference(
        plane.normal, point, plane.point
    );
    return geometry3d_plane_numeric_detail::exact_dot_sign(product) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ON_PLANE_HPP_INCLUDED
