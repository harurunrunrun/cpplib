#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PICK_THEOREM_COUNTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PICK_THEOREM_COUNTS_HPP_INCLUDED

#include <concepts>
#include <vector>

#include "lattice_polygon_interior_points.hpp"
#include "pick_theorem_result.hpp"

template<std::integral Coordinate>
inline PickTheoremResult pick_theorem(
    const std::vector<IntegerPoint<Coordinate>>& polygon
){
    return {
        lattice_polygon_boundary_points(polygon),
        lattice_polygon_interior_points(polygon),
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PICK_THEOREM_COUNTS_HPP_INCLUDED
