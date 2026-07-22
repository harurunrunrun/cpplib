#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYGON3_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYGON3_CONTAINS_HPP_INCLUDED

#include "../../2d/predicate/contains.hpp"
#include <algorithm>
#include <cmath>

#include "../scalar/abs.hpp"
#include "../scalar/dot.hpp"
#include "../point_collection/polygon3_to_2d.hpp"

inline int polygon3_contains(const Polygon3& polygon, const Point3& point){
    const auto [system, projected] = polygon3_to_2d(polygon);
    const long double height = dot(point - system.origin, system.normal);
    if(std::abs(height) > GEOMETRY3D_EPS *
        std::max(1.0L, abs(point - system.origin))){
        return 0;
    }
    return contains(projected, to_plane_coordinates(system, point));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYGON3_CONTAINS_HPP_INCLUDED
