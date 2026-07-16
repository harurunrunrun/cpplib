#pragma once

#include "lattice_polygon_boundary_points.hpp"

template<std::integral Coordinate>
inline __uint128_t lattice_polygon_interior_points(
    const std::vector<IntegerPoint<Coordinate>>& polygon
){
    if(polygon.size() < 3){
        throw std::invalid_argument("lattice polygon needs at least three vertices");
    }
    lattice_polygon_detail::Wide doubled_area =
        lattice_polygon_detail::signed_double_area(polygon);
    if(doubled_area < 0) doubled_area = -doubled_area;
    if(doubled_area == 0){
        throw std::invalid_argument("lattice polygon has zero area");
    }
    const __uint128_t boundary = lattice_polygon_boundary_points(polygon);
    const lattice_polygon_detail::Wide numerator =
        doubled_area - lattice_polygon_detail::Wide(boundary) + 2;
    if(numerator < 0){
        throw std::invalid_argument("vertices do not form a simple lattice polygon");
    }
    auto [interior, remainder] = numerator.divmod(2);
    if(remainder != 0){
        throw std::invalid_argument("vertices do not form a simple lattice polygon");
    }
    return lattice_polygon_detail::to_uint128(interior);
}
