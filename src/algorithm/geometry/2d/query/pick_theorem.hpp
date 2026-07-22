#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_PICK_THEOREM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_PICK_THEOREM_HPP_INCLUDED

#include "../scalar/lattice_polygon_interior_points.hpp"

struct PickTheoremResult{
    __uint128_t boundary_points;
    __uint128_t interior_points;
};

template<std::integral Coordinate>
inline PickTheoremResult pick_theorem(
    const std::vector<IntegerPoint<Coordinate>>& polygon
){
    return {
        lattice_polygon_boundary_points(polygon),
        lattice_polygon_interior_points(polygon),
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_PICK_THEOREM_HPP_INCLUDED
