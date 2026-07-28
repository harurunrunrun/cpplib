#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_POINT_TETRAHEDRON_POINT_BARYCENTRIC_COORDINATES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_POINT_TETRAHEDRON_POINT_BARYCENTRIC_COORDINATES_HPP_INCLUDED

#include <array>
#include <cmath>
#include <stdexcept>

#include "../../detail/point_point_point/barycentric_coordinates_detail.hpp"
#include "../../aggregate/all/is_finite.hpp"
#include "../../type/definition/point3.hpp"
#include "../point_point_point/scalar_triple.hpp"
#include "../../type/definition/tetrahedron3.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_TETRAHEDRON_POINT_TETRAHEDRON_POINT_BARYCENTRIC_COORDINATES_HPP_INCLUDED
