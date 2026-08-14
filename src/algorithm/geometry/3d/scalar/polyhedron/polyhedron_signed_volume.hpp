#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_SIGNED_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_SIGNED_VOLUME_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"
#include "../point_point_point/scalar_triple.hpp"

inline long double polyhedron_signed_volume(const Polyhedron3& polyhedron){
    geometry3d_validate(polyhedron);
    if(polyhedron.vertices.empty()) return 0.0L;
    long double scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L) return 0.0L;
    const Point3 reference = polyhedron.vertices.front() / scale;
    long double scaled_volume = 0.0L;
    for(const auto& face: polyhedron.faces){
        const Point3 first = polyhedron.vertices[face[0]] / scale - reference;
        const Point3 second = polyhedron.vertices[face[1]] / scale - reference;
        const Point3 third = polyhedron.vertices[face[2]] / scale - reference;
        scaled_volume += scalar_triple(first, second, third) / 6.0L;
    }
    const long double result = scaled_volume * scale * scale * scale;
    if(!std::isfinite(result))[[unlikely]]{
        throw std::overflow_error("polyhedron signed volume overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_POLYHEDRON_POLYHEDRON_SIGNED_VOLUME_HPP_INCLUDED
