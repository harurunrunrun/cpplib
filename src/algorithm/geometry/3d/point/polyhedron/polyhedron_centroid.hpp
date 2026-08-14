#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POLYHEDRON_CENTROID_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POLYHEDRON_CENTROID_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "../../predicate/point/point3_is_finite.hpp"
#include "../../scalar/point_point_point/scalar_triple.hpp"
#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"

inline Point3 polyhedron_centroid(const Polyhedron3& polyhedron){
    geometry3d_validate(polyhedron);
    if(polyhedron.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("centroid of empty polyhedron");
    }
    long double scale = 0.0L;
    for(const Point3& point: polyhedron.vertices){
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L)[[unlikely]]{
        throw std::domain_error("zero-volume polyhedron centroid");
    }
    std::vector<Point3> vertices;
    vertices.reserve(polyhedron.vertices.size());
    for(const Point3& point: polyhedron.vertices) vertices.push_back(point / scale);
    const Point3 reference = vertices.front();
    Point3 weighted{};
    long double total = 0.0L;
    for(const auto& face: polyhedron.faces){
        const long double weight = scalar_triple(
            vertices[face[0]] - reference,
            vertices[face[1]] - reference,
            vertices[face[2]] - reference
        );
        weighted += (
            reference + vertices[face[0]] + vertices[face[1]] + vertices[face[2]]
        ) * (weight / 4.0L);
        total += weight;
    }
    if(total == 0.0L)[[unlikely]]{
        throw std::domain_error("zero-volume polyhedron centroid");
    }
    Point3 result = weighted * (scale / total);
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error("polyhedron centroid overflow");
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_POLYHEDRON_POLYHEDRON_CENTROID_HPP_INCLUDED
