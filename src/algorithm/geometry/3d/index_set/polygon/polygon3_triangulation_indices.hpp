#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYGON_POLYGON3_TRIANGULATION_INDICES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYGON_POLYGON3_TRIANGULATION_INDICES_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <vector>

#include "../../../2d/triangle/point_set/simple_polygon_triangulation.hpp"
#include "../../type/definition/polygon3_to_2d_without_plane_coordinate_system.hpp"

inline std::vector<std::array<std::size_t, 3>> polygon3_triangulation_indices(
    const Polygon3& polygon
){
    return simple_polygon_triangulation(polygon3_to_2d(polygon).second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYGON_POLYGON3_TRIANGULATION_INDICES_HPP_INCLUDED
