#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_ANGLE_HPP_INCLUDED

#include <cmath>

#include "../core/geometry_primitives.hpp"
#include "../point/cross.hpp"
#include "dot.hpp"
#include "../point/unit.hpp"

inline long double angle(const Point3& first, const Point3& second){
    const Point3 first_unit = unit(first);
    const Point3 second_unit = unit(second);
    const Point3 product = cross(first_unit, second_unit);
    return std::atan2(
        std::hypot(product.x, product.y, product.z),
        dot(first_unit, second_unit)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_ANGLE_HPP_INCLUDED
