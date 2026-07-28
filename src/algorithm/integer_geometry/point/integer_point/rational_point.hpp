#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_INTEGER_POINT_RATIONAL_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_INTEGER_POINT_RATIONAL_POINT_HPP_INCLUDED

#include "../../type/definition/lattice_types.hpp"

namespace integer_geometry{

inline Point rational_point(const IntegerPoint& point){
    return {Rational(point.x), Rational(point.y)};
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_INTEGER_POINT_RATIONAL_POINT_HPP_INCLUDED
