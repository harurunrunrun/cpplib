#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SIGNED_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SIGNED_VOLUME_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "convex_polyhedron3.hpp"
#include "is_finite.hpp"
#include "scalar_triple.hpp"

inline long double convex_polyhedron_signed_volume(
    const ConvexPolyhedron3& polyhedron
){
    if(polyhedron.affine_dimension < 3) return 0.0L;
    if(polyhedron.vertices.empty()) return 0.0L;
    long double scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite convex polyhedron vertex");
        }
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L) return 0.0L;
    const Point3 reference = polyhedron.vertices.front() / scale;
    long double scaled_volume = 0.0L;
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        const Point3 first = polyhedron.vertices[face[0]] / scale - reference;
        const Point3 second = polyhedron.vertices[face[1]] / scale - reference;
        const Point3 third = polyhedron.vertices[face[2]] / scale - reference;
        scaled_volume += scalar_triple(first, second, third) / 6.0L;
    }
    const long double result = scaled_volume * scale * scale * scale;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("convex polyhedron volume overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_SIGNED_VOLUME_HPP_INCLUDED
