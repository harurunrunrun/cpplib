#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_COPLANAR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_COPLANAR_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "polygon3.hpp"

inline bool polygon3_coplanar(const Polygon3& polygon){
    if(polygon.size() < 3)[[unlikely]]{
        throw std::invalid_argument("a polygon needs at least three vertices");
    }
    const Point3 origin = polygon.front();
    Point3 normal{};
    for(std::size_t index = 1; index + 1 < polygon.size(); ++index){
        normal = cross(polygon[index] - origin, polygon[index + 1] - origin);
        if(geometry3d_sign(abs(normal)) != 0) break;
    }
    if(geometry3d_sign(abs(normal)) == 0) return true;
    const long double scale = abs(normal);
    for(const Point3& point: polygon){
        if(std::abs(dot(point - origin, normal))
            > GEOMETRY3D_EPS * std::max(1.0L, scale * abs(point - origin))){
            return false;
        }
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POLYGON3_COPLANAR_HPP_INCLUDED
