#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "plane_coordinate_system.hpp"
#include "polygon3.hpp"
#include "polygon3_normal.hpp"
#include "to_plane_coordinates.hpp"

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
