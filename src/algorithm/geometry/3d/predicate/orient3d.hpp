#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ORIENT3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ORIENT3D_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "is_finite.hpp"

namespace geometry3d_predicate_detail{

struct NormalizedOrient3D{
    long double determinant = 0.0L;
    long double scale = 0.0L;
    long double permanent = 0.0L;
};

inline NormalizedOrient3D normalized_orient3d(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    const Point3 u = second - first;
    const Point3 v = third - first;
    const Point3 w = fourth - first;
    if(!geometry3d_is_finite(u) || !geometry3d_is_finite(v)
        || !geometry3d_is_finite(w))[[unlikely]]{
        throw std::overflow_error("3D orientation coordinate difference overflow");
    }
    const long double scale = std::max({
        std::abs(u.x), std::abs(u.y), std::abs(u.z),
        std::abs(v.x), std::abs(v.y), std::abs(v.z),
        std::abs(w.x), std::abs(w.y), std::abs(w.z)
    });
    if(scale == 0.0L) return {};
    const Point3 a = u / scale;
    const Point3 b = v / scale;
    const Point3 c = w / scale;
    const long double positive =
        a.x * b.y * c.z + a.y * b.z * c.x + a.z * b.x * c.y;
    const long double negative =
        a.z * b.y * c.x + a.y * b.x * c.z + a.x * b.z * c.y;
    return {
        positive - negative,
        scale,
        std::abs(a.x * b.y * c.z) + std::abs(a.y * b.z * c.x)
            + std::abs(a.z * b.x * c.y) + std::abs(a.z * b.y * c.x)
            + std::abs(a.y * b.x * c.z) + std::abs(a.x * b.z * c.y),
    };
}

inline long double restore_homogeneous_scale(
    long double normalized_value,
    long double scale,
    int degree
){
    if(normalized_value == 0.0L || scale == 0.0L) return 0.0L;
    int exponent = 0;
    const long double mantissa = std::frexp(scale, &exponent);
    long double scaled = normalized_value;
    for(int index = 0; index < degree; ++index) scaled *= mantissa;
    const long long restored_exponent = static_cast<long long>(exponent) * degree;
    if(restored_exponent > std::numeric_limits<int>::max()
        || restored_exponent < std::numeric_limits<int>::min())[[unlikely]]{
        throw std::overflow_error("3D predicate result exponent overflow");
    }
    const long double result = std::ldexp(scaled, static_cast<int>(restored_exponent));
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error("3D predicate result overflow");
    }
    return result;
}

}  // namespace geometry3d_predicate_detail

inline long double orient3d(
    const Point3& first,
    const Point3& second,
    const Point3& third,
    const Point3& fourth
){
    if(!geometry3d_is_finite(first) || !geometry3d_is_finite(second)
        || !geometry3d_is_finite(third) || !geometry3d_is_finite(fourth))[[unlikely]]{
        throw std::invalid_argument("non-finite point in orient3d");
    }
    const auto normalized = geometry3d_predicate_detail::normalized_orient3d(
        first, second, third, fourth
    );
    return geometry3d_predicate_detail::restore_homogeneous_scale(
        normalized.determinant, normalized.scale, 3
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_ORIENT3D_HPP_INCLUDED
