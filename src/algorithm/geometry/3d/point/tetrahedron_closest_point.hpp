#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TETRAHEDRON_CLOSEST_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TETRAHEDRON_CLOSEST_POINT_HPP_INCLUDED

#include <array>
#include <limits>

#include "../core/geometry_primitives.hpp"
#include "closest_point.hpp"
#include "../predicate/contains.hpp"
#include "../scalar/norm.hpp"
#include "../predicate/tetrahedron_orientation.hpp"

inline Point3 closest_point(
    const Tetrahedron3& tetrahedron,
    const Point3& point
){
    if(tetrahedron_orientation(tetrahedron) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    if(contains(tetrahedron, point)) return point;
    const std::array<Triangle3, 4> faces{{
        {tetrahedron.b, tetrahedron.c, tetrahedron.d},
        {tetrahedron.a, tetrahedron.c, tetrahedron.d},
        {tetrahedron.a, tetrahedron.b, tetrahedron.d},
        {tetrahedron.a, tetrahedron.b, tetrahedron.c},
    }};
    long double best_distance = std::numeric_limits<long double>::infinity();
    Point3 best;
    for(const Triangle3& face: faces){
        const Point3 candidate = closest_point(face, point);
        const long double squared_distance = norm(candidate - point);
        if(squared_distance < best_distance){
            best_distance = squared_distance;
            best = candidate;
        }
    }
    return best;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_TETRAHEDRON_CLOSEST_POINT_HPP_INCLUDED
