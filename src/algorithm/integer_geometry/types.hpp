#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_TYPES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_TYPES_HPP_INCLUDED

#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace integer_geometry{

using Coordinate = std::int64_t;
using Wide = __int128_t;
using UnsignedWide = __uint128_t;

namespace detail{

inline Wide checked_add(Wide first, Wide second){
    Wide result;
    if(__builtin_add_overflow(first, second, &result)){
        throw std::overflow_error("signed __int128 addition overflow");
    }
    return result;
}

inline Wide checked_subtract(Wide first, Wide second){
    Wide result;
    if(__builtin_sub_overflow(first, second, &result)){
        throw std::overflow_error("signed __int128 subtraction overflow");
    }
    return result;
}

inline Wide checked_multiply(Wide first, Wide second){
    Wide result;
    if(__builtin_mul_overflow(first, second, &result)){
        throw std::overflow_error("signed __int128 multiplication overflow");
    }
    return result;
}

inline Wide checked_negate(Wide value){
    return checked_subtract(Wide{0}, value);
}

inline UnsignedWide checked_add_unsigned(
    UnsignedWide first,
    UnsignedWide second
){
    UnsignedWide result;
    if(__builtin_add_overflow(first, second, &result)){
        throw std::overflow_error("unsigned __int128 addition overflow");
    }
    return result;
}
}  // namespace detail


struct Point{
    Coordinate x = 0;
    Coordinate y = 0;

    friend bool operator==(const Point&, const Point&) = default;
    friend bool operator<(const Point& left, const Point& right){
        return left.x < right.x || (left.x == right.x && left.y < right.y);
    }
};

struct Vector{
    Wide x = 0;
    Wide y = 0;

    friend bool operator==(const Vector&, const Vector&) = default;
    friend Vector operator+(const Vector& left, const Vector& right){
        return {detail::checked_add(left.x, right.x), detail::checked_add(left.y, right.y)};
    }
    friend Vector operator-(const Vector& left, const Vector& right){
        return {
            detail::checked_subtract(left.x, right.x),
            detail::checked_subtract(left.y, right.y),
        };
    }
    friend Vector operator-(const Vector& value){
        return {detail::checked_negate(value.x), detail::checked_negate(value.y)};
    }
    friend Vector operator*(const Vector& value, Wide scalar){
        return {
            detail::checked_multiply(value.x, scalar),
            detail::checked_multiply(value.y, scalar),
        };
    }
};

struct Segment{
    Point a;
    Point b;
};

struct Line{
    Point a;
    Point b;
};

inline Vector vector_from(const Point& from, const Point& to){
    return {
        static_cast<Wide>(to.x) - static_cast<Wide>(from.x),
        static_cast<Wide>(to.y) - static_cast<Wide>(from.y),
    };
}
namespace detail{


inline UnsignedWide unsigned_magnitude(Wide value){
    const UnsignedWide converted = static_cast<UnsignedWide>(value);
    return value < 0 ? UnsignedWide{0} - converted : converted;
}

inline UnsignedWide gcd_wide(UnsignedWide first, UnsignedWide second){
    while(second != 0){
        const UnsignedWide remainder = first % second;
        first = second;
        second = remainder;
    }
    return first;
}

}  // namespace detail

struct Rational{
    Wide numerator = 0;
    Wide denominator = 1;

    Rational() = default;
    Rational(Wide numerator_value, Wide denominator_value)
        : numerator(numerator_value), denominator(denominator_value){
        if(denominator == 0){
            throw std::invalid_argument("rational denominator must be nonzero");
        }
        const bool negative = numerator != 0
            && ((numerator < 0) != (denominator < 0));
        UnsignedWide numerator_magnitude =
            detail::unsigned_magnitude(numerator);
        UnsignedWide denominator_magnitude =
            detail::unsigned_magnitude(denominator);
        const UnsignedWide divisor = detail::gcd_wide(
            numerator_magnitude, denominator_magnitude
        );
        numerator_magnitude /= divisor;
        denominator_magnitude /= divisor;

        constexpr UnsignedWide signed_maximum =
            static_cast<UnsignedWide>(std::numeric_limits<Wide>::max());
        constexpr UnsignedWide signed_minimum_magnitude =
            signed_maximum + 1;
        if(denominator_magnitude > signed_maximum
            || (!negative && numerator_magnitude > signed_maximum)
            || (negative
                && numerator_magnitude > signed_minimum_magnitude)){
            throw std::overflow_error(
                "normalized rational is not representable");
        }
        denominator = static_cast<Wide>(denominator_magnitude);
        if(!negative){
            numerator = static_cast<Wide>(numerator_magnitude);
        }else if(numerator_magnitude == signed_minimum_magnitude){
            numerator = std::numeric_limits<Wide>::min();
        }else{
            numerator = -static_cast<Wide>(numerator_magnitude);
        }
    }

    friend bool operator==(const Rational&, const Rational&) = default;
};

struct RationalPoint{
    Rational x;
    Rational y;

    friend bool operator==(const RationalPoint&, const RationalPoint&) = default;
};

enum class SegmentIntersectionKind{
    none,
    touch,
    proper,
    overlap,
};

enum class PointLocation{
    outside,
    boundary,
    inside,
};

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_TYPES_HPP_INCLUDED
