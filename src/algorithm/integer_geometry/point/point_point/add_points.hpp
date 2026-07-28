#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_POINT_POINT_ADD_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_POINT_POINT_ADD_POINTS_HPP_INCLUDED

#include "../../type/definition/geometry_types.hpp"

namespace integer_geometry{

inline Point add_points(const Point& first, const Point& second){
    return {first.x + second.x, first.y + second.y};
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_POINT_POINT_ADD_POINTS_HPP_INCLUDED
