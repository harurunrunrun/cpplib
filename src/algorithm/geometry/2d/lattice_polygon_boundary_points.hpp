#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LATTICE_POLYGON_BOUNDARY_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LATTICE_POLYGON_BOUNDARY_POINTS_HPP_INCLUDED

#include "lattice_polygon_detail.hpp"

template<std::integral Coordinate>
inline __uint128_t lattice_polygon_boundary_points(
    const std::vector<IntegerPoint<Coordinate>>& polygon
){
    __uint128_t result = 0;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const auto& current = polygon[index];
        const auto& next = polygon[(index + 1) % polygon.size()];
        result += lattice_polygon_detail::gcd(
            lattice_polygon_detail::unsigned_difference(current.x, next.x),
            lattice_polygon_detail::unsigned_difference(current.y, next.y)
        );
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LATTICE_POLYGON_BOUNDARY_POINTS_HPP_INCLUDED
