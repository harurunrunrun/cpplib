#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ADAPTIVE_ORIENT3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ADAPTIVE_ORIENT3D_HPP_INCLUDED

#include <cmath>
#include <limits>
#include <stdexcept>

#include "../../math/integer/exact_integer.hpp"

#include "expansion_arithmetic.hpp"
#include "is_finite.hpp"
#include "orient3d.hpp"

namespace geometry3d_adaptive_detail{

struct ExactDyadic{
    ExactInteger significand = 0;
    int exponent = 0;
};

inline ExactDyadic exact_dyadic(long double value){
    if(!geometry3d_is_finite(value))[[unlikely]]{
        throw std::invalid_argument("non-finite exact dyadic value");
    }
    if(value == 0.0L) return {};
    const bool negative = std::signbit(value);
    int exponent = 0;
    long double fraction = std::frexp(std::abs(value), &exponent);
    constexpr int digits = std::numeric_limits<long double>::digits;
    ExactInteger significand = 0;
    for(int bit = 0; bit < digits; ++bit){
        fraction = std::ldexp(fraction, 1);
        significand <<= 1;
        if(fraction >= 1.0L){
            significand += 1;
            fraction -= 1.0L;
        }
    }
    if(negative) significand = -significand;
    return {significand, exponent - digits};
}

inline ExactDyadic add(const ExactDyadic& first, const ExactDyadic& second){
    if(first.significand == 0) return second;
    if(second.significand == 0) return first;
    const int exponent = std::min(first.exponent, second.exponent);
    return {
        (first.significand << static_cast<unsigned>(first.exponent - exponent))
            + (second.significand << static_cast<unsigned>(second.exponent - exponent)),
        exponent,
    };
}

inline ExactDyadic negate(ExactDyadic value){
    value.significand = -value.significand;
    return value;
}

inline ExactDyadic subtract(const ExactDyadic& first, const ExactDyadic& second){
    return add(first, negate(second));
}

inline ExactDyadic multiply(const ExactDyadic& first, const ExactDyadic& second){
    return {
        first.significand * second.significand,
        first.exponent + second.exponent,
    };
}

inline int sign(const ExactDyadic& value){
    return value.significand > 0 ? 1 : value.significand < 0 ? -1 : 0;
}

inline int exact_orient3d_sign(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    const ExactDyadic ux = subtract(exact_dyadic(second.x), exact_dyadic(first.x));
    const ExactDyadic uy = subtract(exact_dyadic(second.y), exact_dyadic(first.y));
    const ExactDyadic uz = subtract(exact_dyadic(second.z), exact_dyadic(first.z));
    const ExactDyadic vx = subtract(exact_dyadic(third.x), exact_dyadic(first.x));
    const ExactDyadic vy = subtract(exact_dyadic(third.y), exact_dyadic(first.y));
    const ExactDyadic vz = subtract(exact_dyadic(third.z), exact_dyadic(first.z));
    const ExactDyadic wx = subtract(exact_dyadic(fourth.x), exact_dyadic(first.x));
    const ExactDyadic wy = subtract(exact_dyadic(fourth.y), exact_dyadic(first.y));
    const ExactDyadic wz = subtract(exact_dyadic(fourth.z), exact_dyadic(first.z));
    const ExactDyadic first_term = multiply(
        ux, subtract(multiply(vy, wz), multiply(vz, wy))
    );
    const ExactDyadic second_term = multiply(
        uy, subtract(multiply(vx, wz), multiply(vz, wx))
    );
    const ExactDyadic third_term = multiply(
        uz, subtract(multiply(vx, wy), multiply(vy, wx))
    );
    return sign(add(subtract(first_term, second_term), third_term));
}

inline int expansion_orient3d_sign(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    const Geometry3DExpansion ux = geometry3d_two_diff(second.x, first.x);
    const Geometry3DExpansion uy = geometry3d_two_diff(second.y, first.y);
    const Geometry3DExpansion uz = geometry3d_two_diff(second.z, first.z);
    const Geometry3DExpansion vx = geometry3d_two_diff(third.x, first.x);
    const Geometry3DExpansion vy = geometry3d_two_diff(third.y, first.y);
    const Geometry3DExpansion vz = geometry3d_two_diff(third.z, first.z);
    const Geometry3DExpansion wx = geometry3d_two_diff(fourth.x, first.x);
    const Geometry3DExpansion wy = geometry3d_two_diff(fourth.y, first.y);
    const Geometry3DExpansion wz = geometry3d_two_diff(fourth.z, first.z);
    const auto difference = [](const Geometry3DExpansion& left,
                               const Geometry3DExpansion& right){
        return geometry3d_expansion_sum(
            left, geometry3d_expansion_negate(right)
        );
    };
    const Geometry3DExpansion first_term = geometry3d_expansion_product(
        ux,
        difference(
            geometry3d_expansion_product(vy, wz),
            geometry3d_expansion_product(vz, wy)
        )
    );
    const Geometry3DExpansion second_term = geometry3d_expansion_product(
        uy,
        difference(
            geometry3d_expansion_product(vx, wz),
            geometry3d_expansion_product(vz, wx)
        )
    );
    const Geometry3DExpansion third_term = geometry3d_expansion_product(
        uz,
        difference(
            geometry3d_expansion_product(vx, wy),
            geometry3d_expansion_product(vy, wx)
        )
    );
    return geometry3d_expansion_sign(geometry3d_expansion_sum(
        difference(first_term, second_term), third_term
    ));
}

}  // namespace geometry3d_adaptive_detail

inline int adaptive_orient3d(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second)
        || !geometry3d_is_finite(third) || !geometry3d_is_finite(fourth))[[unlikely]]{
        throw std::invalid_argument("non-finite point in adaptive_orient3d");
    }
    try{
        const auto normalized = geometry3d_predicate_detail::normalized_orient3d(
            first, second, third, fourth
        );
        const long double error_bound = 16.0L
            * std::numeric_limits<long double>::epsilon() * normalized.permanent;
        if(std::abs(normalized.determinant) > error_bound){
            return normalized.determinant > 0.0L ? 1 : -1;
        }
    }catch(const std::overflow_error&){
        // The exact dyadic fallback does not form overflowing coordinate differences.
    }
    int expansion_sign = 0;
    try{
        expansion_sign = geometry3d_adaptive_detail::expansion_orient3d_sign(
            first, second, third, fourth
        );
    }catch(const std::overflow_error&){
        expansion_sign = 0;
    }
    const int exact_sign = geometry3d_adaptive_detail::exact_orient3d_sign(
        first, second, third, fourth
    );
    return expansion_sign == exact_sign ? expansion_sign : exact_sign;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ADAPTIVE_ORIENT3D_HPP_INCLUDED
