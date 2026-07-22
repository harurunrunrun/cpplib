#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ORIENT_POLYGON3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ORIENT_POLYGON3_HPP_INCLUDED

#include <algorithm>

#include "polygon3_signed_area.hpp"

inline Polygon3 orient_polygon3(
    Polygon3 polygon,
    const Point3& reference_normal
){
    if(polygon3_signed_area(polygon, reference_normal) < 0){
        std::reverse(polygon.begin() + 1, polygon.end());
    }
    return polygon;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_ORIENT_POLYGON3_HPP_INCLUDED
