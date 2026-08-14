#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_SURFACE_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_SURFACE_AREA_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../point/point_point/cross.hpp"
#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"

inline long double polyhedron_surface_area(const Polyhedron3& polyhedron){
    geometry3d_validate(polyhedron);
    long double scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L) return 0.0L;
    long double scaled_area = 0.0L;
    for(const auto& face: polyhedron.faces){
        const Point3 first = polyhedron.vertices[face[0]] / scale;
        const Point3 second = polyhedron.vertices[face[1]] / scale;
        const Point3 third = polyhedron.vertices[face[2]] / scale;
        const Point3 product = cross(second - first, third - first);
        scaled_area += std::hypot(product.x, product.y, product.z) / 2.0L;
    }
    const long double result = scaled_area * scale * scale;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("polyhedron surface area overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_SURFACE_AREA_HPP_INCLUDED
