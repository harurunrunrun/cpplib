#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_DOMINANT_AXIS_FROM_POINT3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_DOMINANT_AXIS_FROM_POINT3_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../../scalar/point/abs.hpp"
#include "../../type/definition/dominant_axis3.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"

inline DominantAxis3 dominant_axis(const Point3& normal){
    if(geometry3d_sign(abs(normal)) == 0)[[unlikely]]{
        throw std::invalid_argument("zero normal");
    }
    const long double x = std::abs(normal.x);
    const long double y = std::abs(normal.y);
    const long double z = std::abs(normal.z);
    if(x >= y && x >= z) return DominantAxis3::x;
    if(y >= z) return DominantAxis3::y;
    return DominantAxis3::z;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_POINT_DOMINANT_AXIS_FROM_POINT3_HPP_INCLUDED
