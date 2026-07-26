#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_TRIANGULATION_INDICES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_TRIANGULATION_INDICES_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "../2d/simple_polygon_triangulation.hpp"
#include "polygon3_to_2d.hpp"

inline std::vector<std::array<std::size_t, 3>> polygon3_triangulation_indices(
    const Polygon3& polygon
){
    return simple_polygon_triangulation(polygon3_to_2d(polygon).second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_TRIANGULATION_INDICES_HPP_INCLUDED
