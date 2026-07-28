#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_TO_2D_WITH_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_TO_2D_WITH_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED

#include <vector>

#include "../../aggregate/all/plane_coordinate_system.hpp"
#include "polygon3.hpp"
#include "../../point/point/to_plane_coordinates.hpp"

inline std::vector<Point> polygon3_to_2d(
    const Polygon3& polygon,
    const PlaneCoordinateSystem3& system
){
    std::vector<Point> result;
    result.reserve(polygon.size());
    for(const Point3& point: polygon){
        result.push_back(to_plane_coordinates(system, point));
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_POLYGON3_TO_2D_WITH_PLANE_COORDINATE_SYSTEM_HPP_INCLUDED
