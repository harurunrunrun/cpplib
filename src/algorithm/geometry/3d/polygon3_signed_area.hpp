#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "polygon3_area_vector.hpp"

inline long double polygon3_signed_area(
    const Polygon3& polygon,
    const Point3& reference_normal
){
    const long double length = abs(reference_normal);
    if(geometry3d_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("zero reference normal");
    }
    return dot(polygon3_area_vector(polygon), reference_normal / length);
}
