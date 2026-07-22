#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_LINEAR_INTERSECTION3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_LINEAR_INTERSECTION3_HPP_INCLUDED

#include <variant>

#include "../core/geometry_primitives.hpp"

using LinearIntersection3 =
    std::variant<std::monostate, Point3, Segment3, Ray3, Line3>;

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_LINEAR_INTERSECTION3_HPP_INCLUDED
