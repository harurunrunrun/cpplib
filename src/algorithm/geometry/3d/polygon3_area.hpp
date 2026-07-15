#pragma once

#include "abs.hpp"
#include "polygon3_area_vector.hpp"

inline long double polygon3_area(const Polygon3& polygon){
    return abs(polygon3_area_vector(polygon));
}
