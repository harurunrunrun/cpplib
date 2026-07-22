#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_AREA_HPP_INCLUDED

#include "abs.hpp"
#include "polygon3_area_vector.hpp"

inline long double polygon3_area(const Polygon3& polygon){
    return abs(polygon3_area_vector(polygon));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_AREA_HPP_INCLUDED
