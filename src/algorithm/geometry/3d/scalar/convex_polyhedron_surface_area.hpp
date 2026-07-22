#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_SURFACE_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_SURFACE_AREA_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../core/convex_polyhedron3.hpp"
#include "../point/cross.hpp"
#include "../predicate/is_finite.hpp"

inline long double convex_polyhedron_surface_area(
    const ConvexPolyhedron3& polyhedron
){
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
    long double scaled_area = 0.0L;
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        const Point3 first = polyhedron.vertices[face[0]] / scale;
        const Point3 second = polyhedron.vertices[face[1]] / scale;
        const Point3 third = polyhedron.vertices[face[2]] / scale;
        const Point3 product = cross(second - first, third - first);
        scaled_area += std::hypot(product.x, product.y, product.z) / 2.0L;
    }
    const long double result = scaled_area * scale * scale;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("convex polyhedron surface area overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_CONVEX_POLYHEDRON_SURFACE_AREA_HPP_INCLUDED
