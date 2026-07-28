#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYGON_POINT_POLYGON3_SIGNED_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYGON_POINT_POLYGON3_SIGNED_AREA_HPP_INCLUDED

#include <stdexcept>

#include "../point/abs.hpp"
#include "../point_point/dot.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"
#include "../../point/polygon/polygon3_area_vector.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYGON_POINT_POLYGON3_SIGNED_AREA_HPP_INCLUDED
