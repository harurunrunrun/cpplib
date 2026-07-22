#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_CONTAINS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_CONTAINS_HPP_INCLUDED

#include <stdexcept>

#include "adaptive_orient3d.hpp"
#include "contains.hpp"
#include "convex_polyhedron3.hpp"
#include "distance.hpp"
#include "is_finite.hpp"
#include "on_segment.hpp"

inline bool convex_polyhedron_contains(
    const ConvexPolyhedron3& polyhedron,
    const Point3& point
){
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("non-finite convex polyhedron query point");
    }
    if(polyhedron.vertices.empty()) return false;
    if(polyhedron.affine_dimension <= 0){
        return distance(polyhedron.vertices.front(), point) <= GEOMETRY3D_EPS;
    }
    if(polyhedron.affine_dimension == 1){
        return on_segment({polyhedron.vertices.front(), polyhedron.vertices.back()}, point);
    }
    if(polyhedron.affine_dimension == 2){
        for(const auto& face: polyhedron.faces){
            for(std::size_t vertex: face){
                if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                    throw std::out_of_range("convex polyhedron face index");
                }
            }
            if(contains(Triangle3{
                polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
                polyhedron.vertices[face[2]]
            }, point)) return true;
        }
        return false;
    }
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        if(adaptive_orient3d(
            polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
            polyhedron.vertices[face[2]], point
        ) > 0) return false;
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_CONTAINS_HPP_INCLUDED
