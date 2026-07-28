#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_POINT_SQUARED_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_POINT_SQUARED_DISTANCE_HPP_INCLUDED

#include "../vector/squared_norm.hpp"

namespace integer_geometry{

inline Rational squared_distance(const Point& first, const Point& second){
    return squared_norm(first - second);
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_SCALAR_POINT_POINT_SQUARED_DISTANCE_HPP_INCLUDED
