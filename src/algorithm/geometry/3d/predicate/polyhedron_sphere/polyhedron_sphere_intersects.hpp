#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_SPHERE_POLYHEDRON_SPHERE_INTERSECTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_SPHERE_POLYHEDRON_SPHERE_INTERSECTS_HPP_INCLUDED

#include <stdexcept>

#include "../../scalar/polyhedron_point/polyhedron_point_distance.hpp"
#include "../../type/definition/sphere3.hpp"
#include "../../void/sphere/sphere3_validate.hpp"

inline bool polyhedron_sphere_intersects(
    const Polyhedron3& polyhedron,
    const Sphere3& sphere
){
    geometry3d_validate(sphere);
    if(polyhedron.faces.empty())[[unlikely]]{
        throw std::invalid_argument("sphere intersection with empty polyhedron");
    }
    return polyhedron_point_distance(polyhedron, sphere.center) <= sphere.radius;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_SPHERE_POLYHEDRON_SPHERE_INTERSECTS_HPP_INCLUDED
