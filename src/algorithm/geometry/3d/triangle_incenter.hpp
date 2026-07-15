#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"
#include "circumcircle.hpp"
#include "cross.hpp"
#include "is_finite.hpp"

namespace geometry3d_triangle_incenter_detail{

struct LocalData{
    Point3 offset{};
    long double scale = 1.0L;
    long double normalized_radius = 0.0L;
};

inline Point3 rescale_difference(
    const Geometry3DNormalizedDifference& difference,
    long double scale
){
    const long double ratio = difference.scale / scale;
    const bool nonzero = difference.value.x != 0.0L
        || difference.value.y != 0.0L
        || difference.value.z != 0.0L;
    if(nonzero && ratio == 0.0L)[[unlikely]]{
        throw std::overflow_error(
            "3D triangle local coordinates are not representable"
        );
    }
    return difference.value * ratio;
}

inline LocalData local_data(const Triangle3& triangle){
    using namespace geometry3d_adaptive_detail;
    using namespace geometry3d_circumcircle_detail;
    if(!geometry3d_is_finite(triangle))[[unlikely]]{
        throw std::invalid_argument("triangle incenter requires finite points");
    }
    const auto first_exact =
        geometry3d_plane_numeric_detail::exact_difference(
            triangle.b, triangle.a
        );
    const auto second_exact =
        geometry3d_plane_numeric_detail::exact_difference(
            triangle.c, triangle.a
        );
    if(exact_zero(exact_cross(first_exact, second_exact)))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }

    const Geometry3DNormalizedDifference ab =
        geometry3d_normalized_difference(triangle.b, triangle.a);
    const Geometry3DNormalizedDifference ac =
        geometry3d_normalized_difference(triangle.c, triangle.a);
    const Geometry3DNormalizedDifference bc =
        geometry3d_normalized_difference(triangle.c, triangle.b);
    const long double scale = std::max({
        ab.scale, ac.scale, bc.scale
    });
    const Point3 first = rescale_difference(ab, scale);
    const Point3 second = rescale_difference(ac, scale);
    const Point3 opposite = rescale_difference(bc, scale);
    const long double first_length =
        std::hypot(first.x, first.y, first.z);
    const long double second_length =
        std::hypot(second.x, second.y, second.z);
    const long double opposite_length =
        std::hypot(opposite.x, opposite.y, opposite.z);
    const long double perimeter =
        first_length + second_length + opposite_length;
    const Point3 product = cross(first, second);
    const long double twice_area =
        std::hypot(product.x, product.y, product.z);
    if(perimeter == 0.0L || twice_area == 0.0L)[[unlikely]]{
        throw std::overflow_error(
            "3D triangle local metric is not representable"
        );
    }
    return {
        first * (second_length / perimeter)
            + second * (first_length / perimeter),
        scale,
        twice_area / perimeter,
    };
}

inline long double restore_length(
    long double normalized,
    long double scale,
    const char* message
){
    if(normalized > std::numeric_limits<long double>::max() / scale){
        throw std::overflow_error(message);
    }
    const long double result = normalized * scale;
    if(!std::isfinite(result) || result == 0.0L)[[unlikely]]{
        throw std::overflow_error(message);
    }
    return result;
}

}  // namespace geometry3d_triangle_incenter_detail

inline Point3 triangle_incenter(const Triangle3& triangle){
    const auto data =
        geometry3d_triangle_incenter_detail::local_data(triangle);
    const Point3 result{
        std::fma(data.offset.x, data.scale, triangle.a.x),
        std::fma(data.offset.y, data.scale, triangle.a.y),
        std::fma(data.offset.z, data.scale, triangle.a.z),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "3D triangle incenter is not representable"
        );
    }
    return result;
}
