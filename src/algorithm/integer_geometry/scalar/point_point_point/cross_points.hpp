#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_POINT_POINT_CROSS_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_POINT_POINT_CROSS_POINTS_HPP_INCLUDED

#include "../vector_vector/vector_products.hpp"

namespace integer_geometry{

inline Rational cross(
    const Point& origin,
    const Point& first,
    const Point& second
){
    return cross(first - origin, second - origin);
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_POINT_POINT_CROSS_POINTS_HPP_INCLUDED
