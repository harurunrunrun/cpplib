#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VOID_POLYHEDRON_POLYHEDRON3_VALIDATE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VOID_POLYHEDRON_POLYHEDRON3_VALIDATE_HPP_INCLUDED

#include <stdexcept>

#include "../../predicate/point/point3_is_finite.hpp"
#include "../../type/definition/polyhedron3.hpp"

inline void geometry3d_validate(const Polyhedron3& polyhedron){
    for(const Point3& vertex: polyhedron.vertices){
        if(!geometry3d_is_finite(vertex))[[unlikely]]{
            throw std::invalid_argument("non-finite polyhedron vertex");
        }
    }
    for(const auto& face: polyhedron.faces){
        for(const std::size_t index: face){
            if(index >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("polyhedron face index");
            }
        }
        if(face[0] == face[1] || face[1] == face[2] || face[2] == face[0])
            [[unlikely]]{
            throw std::invalid_argument("polyhedron face repeats a vertex");
        }
    }
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_VOID_POLYHEDRON_POLYHEDRON3_VALIDATE_HPP_INCLUDED
