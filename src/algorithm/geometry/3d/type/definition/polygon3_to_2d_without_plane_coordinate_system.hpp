#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_TO_2D_WITHOUT_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_TO_2D_WITHOUT_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/plane_coordinate_system.hpp"
#include "polygon3.hpp"
#include "../../point/polygon/polygon3_normal.hpp"
#include "polygon3_to_2d_with_plane_coordinate_system.hpp"

inline std::pair<PlaneCoordinateSystem3, std::vector<Point>> polygon3_to_2d(
    const Polygon3& polygon
){
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("a polygon needs at least three vertices");
    }
    const PlaneCoordinateSystem3 system = plane_coordinate_system({
        polygon.front(), polygon3_normal(polygon)
    });
    return {system, polygon3_to_2d(polygon, system)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_TO_2D_WITHOUT_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED
