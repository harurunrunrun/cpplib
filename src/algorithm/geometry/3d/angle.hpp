#pragma once

#include <cmath>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "unit.hpp"

inline long double angle(const Point3& first, const Point3& second){
    const Point3 first_unit = unit(first);
    const Point3 second_unit = unit(second);
    const Point3 product = cross(first_unit, second_unit);
    return std::atan2(
        std::hypot(product.x, product.y, product.z),
        dot(first_unit, second_unit)
    );
}
