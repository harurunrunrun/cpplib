#pragma once

#include <cstddef>
#include <stdexcept>

#include "cross.hpp"
#include "polygon3.hpp"

inline Point3 polygon3_area_vector(const Polygon3& polygon){
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("a polygon needs at least three vertices");
    }
    const Point3 origin = polygon.front();
    Point3 doubled_area{};
    for(std::size_t index = 1; index + 1 < polygon.size(); ++index){
        doubled_area += cross(
            polygon[index] - origin,
            polygon[index + 1] - origin
        );
    }
    return doubled_area / 2.0L;
}
