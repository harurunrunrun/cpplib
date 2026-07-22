#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_PLANAR_INTERSECTION3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_PLANAR_INTERSECTION3_HPP_INCLUDED

#include <variant>

#include "../core/geometry_primitives.hpp"
#include "../core/polygon3.hpp"

using PlanarIntersection3 =
    std::variant<std::monostate, Point3, Segment3, Polygon3>;

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_PLANAR_INTERSECTION3_HPP_INCLUDED
