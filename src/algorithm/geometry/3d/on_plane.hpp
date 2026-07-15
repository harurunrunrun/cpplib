#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <stdexcept>

#include <boost/multiprecision/cpp_bin_float.hpp>

#include "adaptive_orient3d.hpp"
#include "base.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "plane3_unit_normal.hpp"

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

inline NormalizedDyadic normalize_exact_dyadic(
    const geometry3d_adaptive_detail::ExactDyadic& value
){
    using boost::multiprecision::cpp_int;
    if(value.significand == 0) return {};
    cpp_int magnitude = value.significand;
    const bool negative = magnitude < 0;
    if(negative) magnitude = -magnitude;
    const unsigned bit_count =
        boost::multiprecision::msb(magnitude) + 1U;
    constexpr unsigned digits =
        static_cast<unsigned>(std::numeric_limits<long double>::digits);
    const unsigned discarded = bit_count > digits
        ? bit_count - digits : 0U;
    const cpp_int leading = magnitude >> discarded;
    const long double leading_value = leading.convert_to<long double>();
    int leading_exponent = 0;
    long double mantissa = std::frexp(leading_value, &leading_exponent);
    if(negative) mantissa = -mantissa;
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

inline long double exact_ratio(
    const geometry3d_adaptive_detail::ExactDyadic& numerator,
    const geometry3d_adaptive_detail::ExactDyadic& denominator,
    const char* overflow_message
){
    if(denominator.significand == 0)[[unlikely]]{
        throw std::domain_error("exact ratio has zero denominator");
    }
    if(numerator.significand == 0) return 0.0L;
    using boost::multiprecision::cpp_int;
    using HighPrecision = boost::multiprecision::number<
        boost::multiprecision::cpp_bin_float<256>
    >;
    struct LeadingValue{
        HighPrecision value{};
        unsigned discarded = 0;
    };
    const auto leading_value = [](const cpp_int& significand){
        cpp_int magnitude = significand;
        const bool negative = magnitude < 0;
        if(negative) magnitude = -magnitude;
        const unsigned bit_count =
            boost::multiprecision::msb(magnitude) + 1U;
        constexpr unsigned kept_bits = 256;
        const unsigned discarded = bit_count > kept_bits
            ? bit_count - kept_bits : 0U;
        HighPrecision value =
            (magnitude >> discarded).convert_to<HighPrecision>();
        if(negative) value = -value;
        return LeadingValue{value, discarded};
    };
    const LeadingValue normalized_numerator =
        leading_value(numerator.significand);
    const LeadingValue normalized_denominator =
        leading_value(denominator.significand);
    const long long exponent =
        static_cast<long long>(numerator.exponent)
        + static_cast<long long>(normalized_numerator.discarded)
        - static_cast<long long>(denominator.exponent)
        - static_cast<long long>(normalized_denominator.discarded);
    if(exponent < std::numeric_limits<int>::min()
        || exponent > std::numeric_limits<int>::max())[[unlikely]]{
        throw std::overflow_error(overflow_message);
    }
    const long double quotient = (
        normalized_numerator.value / normalized_denominator.value
    ).convert_to<long double>();
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
