#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_BASE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_BASE_HPP_INCLUDED

#include <vector>

#include "../constants.hpp"
#include "../cross.hpp"
#include "../dot.hpp"

enum HalfPlaneIntersectionKind{
    HALF_PLANE_EMPTY,
    HALF_PLANE_BOUNDED,
    HALF_PLANE_UNBOUNDED,
};

struct HalfPlaneIntersectionResult{
    HalfPlaneIntersectionKind kind = HALF_PLANE_EMPTY;
    std::vector<Point> polygon;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ADVANCED_BASE_HPP_INCLUDED
