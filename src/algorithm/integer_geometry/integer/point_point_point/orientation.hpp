#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_INTEGER_POINT_POINT_POINT_ORIENTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_INTEGER_POINT_POINT_POINT_ORIENTATION_HPP_INCLUDED

#include "../../scalar/point_point_point/cross_points.hpp"

namespace integer_geometry{

inline int orientation(
    const Point& first,
    const Point& second,
    const Point& third
){
    return cross(first, second, third).sign();
}

inline int ccw(const Point& first, const Point& second, const Point& third){
    return orientation(first, second, third);
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_INTEGER_POINT_POINT_POINT_ORIENTATION_HPP_INCLUDED
