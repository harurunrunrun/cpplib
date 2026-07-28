#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_HALF_PLANE_INTERSECTION_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_HALF_PLANE_INTERSECTION_RESULT_HPP_INCLUDED

#include <vector>

#include "../../constant/definition/constants.hpp"
#include "../../scalar/point_point/cross.hpp"
#include "../../scalar/point_point/dot.hpp"

enum HalfPlaneIntersectionKind{
    HALF_PLANE_EMPTY,
    HALF_PLANE_BOUNDED,
    HALF_PLANE_UNBOUNDED,
};

struct HalfPlaneIntersectionResult{
    HalfPlaneIntersectionKind kind = HALF_PLANE_EMPTY;
    std::vector<Point> polygon;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_HALF_PLANE_INTERSECTION_RESULT_HPP_INCLUDED
