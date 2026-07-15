#pragma once

#include "../2d/polygon_centroid.hpp"
#include "from_plane_coordinates.hpp"
#include "polygon3_to_2d.hpp"

inline Point3 polygon3_centroid(const Polygon3& polygon){
    const auto [system, projected] = polygon3_to_2d(polygon);
    return from_plane_coordinates(system, polygon_centroid(projected));
}
