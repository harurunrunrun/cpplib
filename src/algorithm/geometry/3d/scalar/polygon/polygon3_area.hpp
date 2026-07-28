#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYGON_POLYGON3_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYGON_POLYGON3_AREA_HPP_INCLUDED

#include "../point/abs.hpp"
#include "../../point/polygon/polygon3_area_vector.hpp"

inline long double polygon3_area(const Polygon3& polygon){
    return abs(polygon3_area_vector(polygon));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYGON_POLYGON3_AREA_HPP_INCLUDED
