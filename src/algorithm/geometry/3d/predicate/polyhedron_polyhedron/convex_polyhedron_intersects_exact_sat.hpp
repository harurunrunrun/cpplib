#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTS_EXACT_SAT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTS_EXACT_SAT_HPP_INCLUDED

#include <vector>

#include "../../detail/polyhedron/convex_polyhedron_intersects_detail.hpp"

inline bool convex_polyhedron_intersects_exact_sat(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    using namespace convex_polyhedron_intersects_detail;
    const ExactPolyhedron exact_first = exact_polyhedron(first);
    const ExactPolyhedron exact_second = exact_polyhedron(second);
    const ExactRelativeVertices relative = exact_relative_vertices(
        exact_first.vertices, exact_second.vertices
    );
    if(face_axis_separates(first, exact_first, relative)
        || face_axis_separates(second, exact_second, relative)) return false;

    std::vector<ExactVector> second_directions;
    second_directions.reserve(exact_second.edges.size());
    for(const auto& second_edge: exact_second.edges){
        second_directions.push_back(exact_difference(
            exact_second.vertices[second_edge[1]],
            exact_second.vertices[second_edge[0]]
        ));
    }
    for(const auto& first_edge: exact_first.edges){
        const ExactVector first_direction = exact_difference(
            exact_first.vertices[first_edge[1]],
            exact_first.vertices[first_edge[0]]
        );
        for(const ExactVector& second_direction: second_directions){
            if(exact_axis_separates(
                exact_cross(first_direction, second_direction), relative
            )) return false;
        }
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_CONVEX_POLYHEDRON_INTERSECTS_EXACT_SAT_HPP_INCLUDED
