#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_PLANAR_INTERSECTION3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_PLANAR_INTERSECTION3_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "polygon3.hpp"

using PlanarIntersection3 =
    std::variant<std::monostate, Point3, Segment3, Polygon3>;

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_PLANAR_INTERSECTION3_HPP_INCLUDED
