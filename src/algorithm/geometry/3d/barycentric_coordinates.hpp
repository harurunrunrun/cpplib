#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "scalar_triple.hpp"

namespace geometry3d_barycentric_detail{

template<std::size_t Size>
inline long double edge_scale(
    const std::array<Point3, Size>& points
){
    long double result = 0.0L;
    for(const Point3& current: points){
        if(!geometry3d_is_finite(current))[[unlikely]]{
            throw std::invalid_argument(
                "barycentric coordinates require finite points"
            );
        }
    }
    for(std::size_t index = 1; index < points.size(); ++index){
        const Geometry3DNormalizedDifference difference =
            geometry3d_normalized_difference(points[index], points.front());
        if(difference.value.x != 0.0L || difference.value.y != 0.0L
            || difference.value.z != 0.0L){
            result = std::max(result, difference.scale);
        }
    }
    return result;
}

inline Point3 normalized_difference(
    const Point3& left,
    const Point3& right,
    long double scale
){
    return geometry3d_safe_normalized_difference(left, right, scale);
}

inline long double vector_scale(const Point3& first, const Point3& second){
    return std::max({
        std::abs(first.x), std::abs(first.y), std::abs(first.z),
        std::abs(second.x), std::abs(second.y), std::abs(second.z),
    });
}

inline long double vector_scale(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    return std::max({
        vector_scale(first, second),
        std::abs(third.x), std::abs(third.y), std::abs(third.z),
    });
}

}  // namespace geometry3d_barycentric_detail

inline std::array<long double, 3> barycentric_coordinates(
    const Triangle3& triangle,
    const Point3& point
){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument(
            "barycentric coordinates require finite points"
        );
    }
    const std::array<Point3, 3> vertices{
        triangle.a, triangle.b, triangle.c,
    };
    const long double edge_scale =
        geometry3d_barycentric_detail::edge_scale(vertices);
    if(edge_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    Point3 first = geometry3d_barycentric_detail::normalized_difference(
        triangle.b, triangle.a, edge_scale
    );
    Point3 second = geometry3d_barycentric_detail::normalized_difference(
        triangle.c, triangle.a, edge_scale
    );
    Point3 offset = geometry3d_barycentric_detail::normalized_difference(
        point, triangle.a, edge_scale
    );
    const long double scale =
        geometry3d_barycentric_detail::vector_scale(first, second);
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    first /= scale;
    second /= scale;
    offset /= scale;
    if(!geometry3d_is_finite(offset))[[unlikely]]{
        throw std::overflow_error("barycentric triangle weights overflow");
    }
    const Point3 normal = cross(first, second);
    const long double denominator = dot(normal, normal);
    if(denominator == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate 3D triangle");
    }
    const long double second_weight =
        dot(cross(offset, second), normal) / denominator;
    const long double third_weight =
        dot(cross(first, offset), normal) / denominator;
    const long double first_weight = 1 - second_weight - third_weight;
    if(!std::isfinite(first_weight) || !std::isfinite(second_weight)
        || !std::isfinite(third_weight))[[unlikely]]{
        throw std::overflow_error("barycentric triangle weights overflow");
    }
    return {first_weight, second_weight, third_weight};
}

inline std::array<long double, 4> barycentric_coordinates(
    const Tetrahedron3& tetrahedron,
    const Point3& point
){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument(
            "barycentric coordinates require finite points"
        );
    }
    const std::array<Point3, 4> vertices{
        tetrahedron.a, tetrahedron.b, tetrahedron.c, tetrahedron.d,
    };
    const long double edge_scale =
        geometry3d_barycentric_detail::edge_scale(vertices);
    if(edge_scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    Point3 first = geometry3d_barycentric_detail::normalized_difference(
        tetrahedron.b, tetrahedron.a, edge_scale
    );
    Point3 second = geometry3d_barycentric_detail::normalized_difference(
        tetrahedron.c, tetrahedron.a, edge_scale
    );
    Point3 third = geometry3d_barycentric_detail::normalized_difference(
        tetrahedron.d, tetrahedron.a, edge_scale
    );
    Point3 offset = geometry3d_barycentric_detail::normalized_difference(
        point, tetrahedron.a, edge_scale
    );
    const long double scale =
        geometry3d_barycentric_detail::vector_scale(first, second, third);
    if(scale == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    first /= scale;
    second /= scale;
    third /= scale;
    offset /= scale;
    if(!geometry3d_is_finite(offset))[[unlikely]]{
        throw std::overflow_error("barycentric tetrahedron weights overflow");
    }
    const long double denominator = scalar_triple(first, second, third);
    if(denominator == 0.0L)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const long double second_weight =
        scalar_triple(offset, second, third) / denominator;
    const long double third_weight =
        scalar_triple(first, offset, third) / denominator;
    const long double fourth_weight =
        scalar_triple(first, second, offset) / denominator;
    const long double first_weight =
        1 - second_weight - third_weight - fourth_weight;
    if(!std::isfinite(first_weight) || !std::isfinite(second_weight)
        || !std::isfinite(third_weight)
        || !std::isfinite(fourth_weight))[[unlikely]]{
        throw std::overflow_error("barycentric tetrahedron weights overflow");
    }
    return {
        first_weight,
        second_weight,
        third_weight,
        fourth_weight,
    };
}
