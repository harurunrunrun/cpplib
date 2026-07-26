#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_EXACT_SAT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_EXACT_SAT_HPP_INCLUDED

#include "convex_polyhedron_intersects_detail.hpp"

inline bool convex_polyhedron_intersects_exact_sat(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    using namespace convex_polyhedron_intersects_detail;
    const ExactPolyhedron exact_first = exact_polyhedron(first);
    const ExactPolyhedron exact_second = exact_polyhedron(second);
    if(face_axis_separates(
        first, exact_first, exact_first, exact_second
    ) || face_axis_separates(
        second, exact_second, exact_first, exact_second
    )) return false;

    for(const auto& first_edge: exact_first.edges){
        const ExactVector first_direction = exact_difference(
            exact_first.vertices[first_edge[1]],
            exact_first.vertices[first_edge[0]]
        );
        for(const auto& second_edge: exact_second.edges){
            const ExactVector second_direction = exact_difference(
                exact_second.vertices[second_edge[1]],
                exact_second.vertices[second_edge[0]]
            );
            if(exact_axis_separates(
                exact_cross(first_direction, second_direction),
                exact_first.vertices, exact_second.vertices
            )) return false;
        }
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_CONVEX_POLYHEDRON_INTERSECTS_EXACT_SAT_HPP_INCLUDED
