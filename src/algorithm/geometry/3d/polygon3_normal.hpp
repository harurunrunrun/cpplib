#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "geometry3d_sign.hpp"
#include "polygon3_area_vector.hpp"

inline Point3 polygon3_normal(const Polygon3& polygon){
    const Point3 vector = polygon3_area_vector(polygon);
    const long double length = abs(vector);
    if(geometry3d_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("a polygon needs nonzero area");
    }
    return vector / length;
}
