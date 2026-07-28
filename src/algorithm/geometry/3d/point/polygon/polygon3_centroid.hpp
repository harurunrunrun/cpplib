#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYGON_POLYGON3_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYGON_POLYGON3_CENTROID_HPP_INCLUDED

#include "../../../2d/point/point_set/polygon_centroid.hpp"
#include "../point/from_plane_coordinates.hpp"
#include "../../type/definition/polygon3_to_2d_without_plane_coordinate_system.hpp"

inline Point3 polygon3_centroid(const Polygon3& polygon){
    const auto [system, projected] = polygon3_to_2d(polygon);
    return from_plane_coordinates(system, polygon_centroid(projected));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYGON_POLYGON3_CENTROID_HPP_INCLUDED
