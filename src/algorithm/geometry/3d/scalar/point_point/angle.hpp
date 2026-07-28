#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_ANGLE_HPP_INCLUDED

#include <cmath>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/point_point/cross.hpp"
#include "dot.hpp"
#include "../../point/point/unit.hpp"

inline long double angle(const Point3& first, const Point3& second){
    const Point3 first_unit = unit(first);
    const Point3 second_unit = unit(second);
    const Point3 product = cross(first_unit, second_unit);
    return std::atan2(
        std::hypot(product.x, product.y, product.z),
        dot(first_unit, second_unit)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POINT_POINT_ANGLE_HPP_INCLUDED
