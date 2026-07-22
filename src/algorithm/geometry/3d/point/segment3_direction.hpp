#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SEGMENT3_DIRECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SEGMENT3_DIRECTION_HPP_INCLUDED

#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "../predicate/is_zero_vector.hpp"

inline Point3 segment3_direction(const Segment3& segment){
    const Point3 direction = segment.b - segment.a;
    if(is_zero_vector(direction))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D segment has no direction");
    }
    return direction;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SEGMENT3_DIRECTION_HPP_INCLUDED
